#include "WaveControl.h"
#include <QMessageBox>

WaveControl::WaveControl(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::WaveControl)
    , m_serialPort(nullptr)
    , m_serialConfigDialog(nullptr)
    , m_serialBaudRate(115200)
    , m_serialDataBits(QSerialPort::Data8)
    , m_serialParity(QSerialPort::NoParity)
    , m_serialStopBits(QSerialPort::OneStop)
{
    ui->setupUi(this);
    m_serialConfigDialog = new SerialConfigDialog(this);
}

WaveControl::~WaveControl()
{
    delete ui;
}

void WaveControl::setDevice(ICommunication* device)
{
    m_serialPort = device;
    if (m_serialPort) {
        connect(m_serialPort, &ICommunication::connectStatus,
            this, &WaveControl::handleOpenSerialResult);
    }
}

void WaveControl::on_btnSerialSetting_clicked()
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

void WaveControl::on_btnOpenSerial_clicked()
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
        ui->btnSerialSetting->setEnabled(true);
    }
}

void WaveControl::handleOpenSerialResult(const QString&, bool result, const QString& errStr)
{
    if (result) {
        ui->btnOpenSerial->setText(QStringLiteral("关闭串口"));
        ui->btnSerialSetting->setEnabled(false);
    }
    else {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("打开串口失败: ") + errStr);
    }
}

QVariantList WaveControl::getSerialParaList() const
{
    return { m_serialPortName, m_serialBaudRate, (int)m_serialDataBits, (int)m_serialParity, (int)m_serialStopBits };
}
