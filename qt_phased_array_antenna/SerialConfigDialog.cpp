#include "SerialConfigDialog.h"
#include "ui_SeialCongigDialog.h"
#include <QMessageBox>
#include <QCollator>
#include <QSerialPortInfo>
#include <QString> 

SerialConfigDialog::SerialConfigDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SerialConfigDialog), 
    m_settings(nullptr)
{
    ui->setupUi(this);

    // 初始化定时器，每秒扫描一次串口设备
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &SerialConfigDialog::refreshPorts);
    m_refreshTimer->start(1000);

    // 初始化控件
    refreshPorts();

    // 加载INI配置
    loadINI();

    // 设置默认值（如果INI中没有配置）
    if (ui->baudrate->count() == 0) {
        QStringList defaultRates = { "1200", "2400", "4800", "9600", "19200", "38400",
                                   "57600", "115200", "230400", "345600", "460800",
                                   "576000", "921600", "1382400" };
        ui->baudrate->addItems(defaultRates);
        ui->baudrate->setCurrentText("9600");
    }

    if (ui->stopbits->count() == 0) {
        QStringList stopBits = { "1", "1.5", "2" };
        ui->stopbits->addItems(stopBits);
        ui->stopbits->setCurrentText("1");
    }

    if (ui->parity->count() == 0) {
        QStringList parityBits = { "None", "Even", "Odd" };
        ui->parity->addItems(parityBits);
        ui->parity->setCurrentText("None");
    }

    if (ui->databits->count() == 0) {
        QStringList dataBits = { "5", "6", "7", "8" };
        ui->databits->addItems(dataBits);
        ui->databits->setCurrentText("8");
    }

    connect(ui->okButton, &QPushButton::clicked, this, &SerialConfigDialog::onAccepted);
}

SerialConfigDialog::~SerialConfigDialog()
{
    delete ui;
    if (m_settings) {
        delete m_settings;
    }
}

void SerialConfigDialog::refreshPorts()
{
    QStringList newPorts;
    static QStringList lastPorts;

    for (const auto& port : QSerialPortInfo::availablePorts()) {
        newPorts << QString(port.portName() + " " + port.description());
    }

    // 仅当端口变化时更新UI
    if (newPorts != lastPorts) {
        lastPorts = newPorts;
        QString currentSelection = ui->serial_name->currentText();
        ui->serial_name->clear();
        ui->serial_name->addItems(lastPorts);

        // 尝试恢复之前的选择
        if (!currentSelection.isEmpty()) {
            int index = ui->serial_name->findText(currentSelection);
            if (index >= 0) {
                ui->serial_name->setCurrentIndex(index);
            }
        }
    }

    // 检查端口是否断开
    if (!m_lastUsedPort.isEmpty()) {
        bool portFound = false;
        for (const auto& port : QSerialPortInfo::availablePorts()) {
            QString portInfo = QString(port.portName() + " " + port.description());
            if (portInfo == m_lastUsedPort) {
                portFound = true;
                break;
            }
        }

        if (!portFound) {
            emit portDisconnected();
            m_lastUsedPort.clear();
        }
    }
}

QString SerialConfigDialog::portName() const
{
    int index = ui->serial_name->currentText().indexOf(' ');
    QString portName = ui->serial_name->currentText().left(index);
    m_lastUsedPort = ui->serial_name->currentText(); // 显式转换为 QString // 记录当前选择的端口
    return portName;
}

int SerialConfigDialog::baudRate() const
{
    return ui->baudrate->currentText().toInt();
}

QSerialPort::DataBits SerialConfigDialog::dataBits() const
{
    QString currentDataBits = ui->databits->currentText();
    if (currentDataBits == "5") return QSerialPort::Data5;
    if (currentDataBits == "6") return QSerialPort::Data6;
    if (currentDataBits == "7") return QSerialPort::Data7;
    return QSerialPort::Data8;
}

QSerialPort::Parity SerialConfigDialog::parity() const
{
    QString currentParity = ui->parity->currentText();
    if (currentParity == "Odd") return QSerialPort::OddParity;
    if (currentParity == "Even") return QSerialPort::EvenParity;
    return QSerialPort::NoParity;
}

QSerialPort::StopBits SerialConfigDialog::stopBits() const
{
    QString currentStopBits = ui->stopbits->currentText();
    if (currentStopBits == "1.5") return QSerialPort::OneAndHalfStop;
    if (currentStopBits == "2") return QSerialPort::TwoStop;
    return QSerialPort::OneStop;
}

bool SerialConfigDialog::hexDisplay() const
{
    return ui->hex_Button->isChecked();
}

void SerialConfigDialog::setCurrentConfig(const QString& port, int baud,
    QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity,
    QSerialPort::StopBits stopBits,
    bool hexDisplay)
{
    // 设置端口
    QString portDisplayName;
    for (int i = 0; i < ui->serial_name->count(); i++) {
        if (ui->serial_name->itemText(i).startsWith(port)) {
            ui->serial_name->setCurrentIndex(i);
            portDisplayName = ui->serial_name->itemText(i);
            break;
        }
    }

    // 记录当前选择的端口
    if (!portDisplayName.isEmpty()) {
        m_lastUsedPort = portDisplayName;
    }

    // 设置波特率
    ui->baudrate->setCurrentText(QString::number(baud));

    // 设置数据位
    QString dataBitsStr;
    switch (dataBits) {
    case QSerialPort::Data5: dataBitsStr = "5"; break;
    case QSerialPort::Data6: dataBitsStr = "6"; break;
    case QSerialPort::Data7: dataBitsStr = "7"; break;
    default: dataBitsStr = "8"; break;
    }
    ui->databits->setCurrentText(dataBitsStr);

    // 设置校验位
    QString parityStr;
    switch (parity) {
    case QSerialPort::OddParity: parityStr = "Odd"; break;
    case QSerialPort::EvenParity: parityStr = "Even"; break;
    default: parityStr = "None"; break;
    }
    ui->parity->setCurrentText(parityStr);

    // 设置停止位
    QString stopBitsStr;
    switch (stopBits) {
    case QSerialPort::OneAndHalfStop: stopBitsStr = "1.5"; break;
    case QSerialPort::TwoStop: stopBitsStr = "2"; break;
    default: stopBitsStr = "1"; break;
    }
    ui->stopbits->setCurrentText(stopBitsStr);

    // 设置显示格式
    if (hexDisplay) {
        ui->hex_Button->setChecked(true);
    }
    else {
        ui->ascii_Button->setChecked(true);
    }
}

void SerialConfigDialog::loadINI()
{
    if (!m_settings) {
        m_settings = new QSettings("config.ini", QSettings::IniFormat, this);
    }

    QStringList defaultRates = { "1200", "2400", "4800", "9600", "19200", "38400",
                               "57600", "115200", "230400", "345600", "460800",
                               "576000", "921600", "1382400" };
    QStringList baudRates = m_settings->value("BaudRates/myBaud", defaultRates).toStringList();

    ui->baudrate->clear();
    ui->baudrate->addItems(baudRates);
}

void SerialConfigDialog::saveINI()
{
    if (!m_settings) {
        m_settings = new QSettings("config.ini", QSettings::IniFormat, this);
    }

    QCollator collator;
    QStringList ratesList;

    for (int i = 0; i < ui->baudrate->count(); i++) {
        ratesList << ui->baudrate->itemText(i);
    }
    collator.setNumericMode(true);
    std::sort(ratesList.begin(), ratesList.end(), collator);
    m_settings->setValue("BaudRates/myBaud", ratesList);
}

void SerialConfigDialog::onAccepted()
{
    // 检查波特率是否存在，不存在则添加
    bool isFound = false;
    QString current_baud = ui->baudrate->currentText();
    for (int i = 0; i < ui->baudrate->count(); i++) {
        if (ui->baudrate->itemText(i) == current_baud) {
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        ui->baudrate->addItem(current_baud);
    }

    // 保存配置
    saveINI();

    accept();
}