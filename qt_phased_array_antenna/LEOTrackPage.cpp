#include "LEOTrackPage.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDateTime>
#include <QRegularExpression>

LEOTrackPage::LEOTrackPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TaskTrajectoryDisplay)
    , m_serialPort(nullptr)
    , m_serialConfigDialog(nullptr)
    , m_serialBaudRate(115200)
    , m_serialDataBits(QSerialPort::Data8)
    , m_serialParity(QSerialPort::NoParity)
    , m_serialStopBits(QSerialPort::OneStop)
    , m_beijingTimeTimer(nullptr)
{
    ui->setupUi(this);
    m_serialConfigDialog = new SerialConfigDialog(this);

    m_beijingTimeTimer = new QTimer(this);
    m_beijingTimeTimer->setInterval(1000);
    connect(m_beijingTimeTimer, &QTimer::timeout, this, &LEOTrackPage::updateBeijingTime);
    updateBeijingTime();
}

LEOTrackPage::~LEOTrackPage()
{
    delete ui;
}

void LEOTrackPage::setDevice(ICommunication* device)
{
    m_serialPort = device;
    if (m_serialPort) {
        connect(m_serialPort, &ICommunication::connectStatus,
            this, &LEOTrackPage::handleOpenSerialResult);
    }
}

void LEOTrackPage::on_btnBrowseFile_clicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择星历文件"),
        QString(),
        QStringLiteral("星历文件 (*.eph)"));

    if (filePath.isEmpty()) return;
    if (!filePath.endsWith(QStringLiteral(".eph"), Qt::CaseInsensitive)) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("仅支持 .eph 文件"));
        return;
    }

    if (!loadEphFile(filePath)) return;
    ui->editFilePath->setText(filePath);
}

void LEOTrackPage::on_btnClearFile_clicked()
{
    ui->editFilePath->clear();
    ui->textBrowserRecvSource->clear();
}

void LEOTrackPage::on_btnRealtime_clicked()
{
    updateBeijingTime();
    m_beijingTimeTimer->start();
}

void LEOTrackPage::on_btnPreset_clicked()
{
    // Reserved for future preset mode.
}

void LEOTrackPage::on_btnSerialConfig_clicked()
{
    if (!m_serialConfigDialog) return;

    m_serialConfigDialog->setCurrentConfig(m_serialPortName, m_serialBaudRate,
        m_serialDataBits, m_serialParity, m_serialStopBits, true);

    if (m_serialConfigDialog->exec() == QDialog::Accepted) {
        m_serialPortName = m_serialConfigDialog->portName();
        m_serialBaudRate = m_serialConfigDialog->baudRate();
        m_serialDataBits = m_serialConfigDialog->dataBits();
        m_serialParity = m_serialConfigDialog->parity();
        m_serialStopBits = m_serialConfigDialog->stopBits();
    }
}

void LEOTrackPage::on_btnOpenSerial_clicked()
{
    if (!m_serialPort) return;

    if (ui->btnOpenSerial->text() == QStringLiteral("打开串口")) {
        if (m_serialPortName.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先配置串口"));
            return;
        }
        m_serialPort->portConnect(getSerialParaList());
    }
    else {
        m_serialPort->disconnect();
        ui->btnOpenSerial->setText(QStringLiteral("打开串口"));
        ui->btnSerialConfig->setEnabled(true);
    }
}

void LEOTrackPage::handleOpenSerialResult(const QString&, bool result, const QString& errStr)
{
    if (result) {
        ui->btnOpenSerial->setText(QStringLiteral("关闭串口"));
        ui->btnSerialConfig->setEnabled(false);
    }
    else {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("打开串口失败: ") + errStr);
    }
}

QVariantList LEOTrackPage::getSerialParaList() const
{
    return { m_serialPortName, m_serialBaudRate, (int)m_serialDataBits, (int)m_serialParity, (int)m_serialStopBits };
}

void LEOTrackPage::updateBeijingTime()
{
    ui->editBeijingTime->setText(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
}

bool LEOTrackPage::loadEphFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("文件打开失败"));
        return false;
    }

    const QByteArray rawData = file.readAll();
    QString content = QString::fromUtf8(rawData);
    if (content.contains(QChar::ReplacementCharacter)) {
        content = QString::fromLocal8Bit(rawData);
    }

    ui->textBrowserRecvSource->setPlainText(content);

    const QStringList lines = content.split(QRegularExpression("[\\r\\n]+"), Qt::SkipEmptyParts);
    QDateTime startTime;
    QDateTime endTime;
    bool hasData = false;

    double minAzimuth = 0.0;
    double maxAzimuth = 0.0;
    double minPitch = 0.0;
    double maxPitch = 0.0;

    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) continue;

        const QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 4) continue;

        const QDateTime time = QDateTime::fromString(parts[0] + QStringLiteral(" ") + parts[1],
            QStringLiteral("yyyy.MM.dd HH:mm:ss"));
        bool okAz = false;
        bool okPitch = false;
        const double azimuth = parts[2].toDouble(&okAz);
        const double pitch = parts[3].toDouble(&okPitch);
        if (!time.isValid() || !okAz || !okPitch) continue;

        if (!hasData) {
            startTime = time;
            endTime = time;
            minAzimuth = maxAzimuth = azimuth;
            minPitch = maxPitch = pitch;
            hasData = true;
            continue;
        }

        endTime = time;
        if (azimuth < minAzimuth) minAzimuth = azimuth;
        if (azimuth > maxAzimuth) maxAzimuth = azimuth;
        if (pitch < minPitch) minPitch = pitch;
        if (pitch > maxPitch) maxPitch = pitch;
    }

    if (!hasData) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("文件中未解析到有效 .eph 数据"));
        return false;
    }

    const qint64 intervalSec = startTime.secsTo(endTime);
    ui->editStartBeijingTime->setText(startTime.toString(QStringLiteral("yyyy.MM.dd HH:mm:ss")));
    ui->editEndBeijingTime->setText(endTime.toString(QStringLiteral("yyyy.MM.dd HH:mm:ss")));
    ui->editTimeInterval->setText(QString::number(intervalSec));

    ui->editPitchRangeMin->setText(QString::number(minPitch, 'f', 3));
    ui->editPitchRangeMax->setText(QString::number(maxPitch, 'f', 3));
    ui->editAzimuthRangeMin->setText(QString::number(minAzimuth, 'f', 3));
    ui->editAzimuthRangeMax->setText(QString::number(maxAzimuth, 'f', 3));

    // Satellite name is intentionally left blank for now.
    ui->editSatelliteName->clear();
    return true;
}
