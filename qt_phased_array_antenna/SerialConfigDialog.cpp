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

    // ��ʼ����ʱ����ÿ��ɨ��һ�δ����豸
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &SerialConfigDialog::refreshPorts);
    m_refreshTimer->start(1000);

    // ��ʼ���ؼ�
    refreshPorts();

    // ����INI����
    loadINI();

    // ����Ĭ��ֵ�����INI��û�����ã�
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

    // �����˿ڱ仯ʱ����UI
    if (newPorts != lastPorts) {
        lastPorts = newPorts;
        QString currentSelection = ui->serial_name->currentText();
        ui->serial_name->clear();
        ui->serial_name->addItems(lastPorts);

        // ���Իָ�֮ǰ��ѡ��
        if (!currentSelection.isEmpty()) {
            int index = ui->serial_name->findText(currentSelection);
            if (index >= 0) {
                ui->serial_name->setCurrentIndex(index);
            }
        }
    }

    // ���˿��Ƿ�Ͽ�
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
    m_lastUsedPort = ui->serial_name->currentText(); // ��ʽת��Ϊ QString // ��¼��ǰѡ��Ķ˿�
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
    // ���ö˿�
    QString portDisplayName;
    for (int i = 0; i < ui->serial_name->count(); i++) {
        if (ui->serial_name->itemText(i).startsWith(port)) {
            ui->serial_name->setCurrentIndex(i);
            portDisplayName = ui->serial_name->itemText(i);
            break;
        }
    }

    // ��¼��ǰѡ��Ķ˿�
    if (!portDisplayName.isEmpty()) {
        m_lastUsedPort = portDisplayName;
    }

    // ���ò�����
    ui->baudrate->setCurrentText(QString::number(baud));

    // ��������λ
    QString dataBitsStr;
    switch (dataBits) {
    case QSerialPort::Data5: dataBitsStr = "5"; break;
    case QSerialPort::Data6: dataBitsStr = "6"; break;
    case QSerialPort::Data7: dataBitsStr = "7"; break;
    default: dataBitsStr = "8"; break;
    }
    ui->databits->setCurrentText(dataBitsStr);

    // ����У��λ
    QString parityStr;
    switch (parity) {
    case QSerialPort::OddParity: parityStr = "Odd"; break;
    case QSerialPort::EvenParity: parityStr = "Even"; break;
    default: parityStr = "None"; break;
    }
    ui->parity->setCurrentText(parityStr);

    // ����ֹͣλ
    QString stopBitsStr;
    switch (stopBits) {
    case QSerialPort::OneAndHalfStop: stopBitsStr = "1.5"; break;
    case QSerialPort::TwoStop: stopBitsStr = "2"; break;
    default: stopBitsStr = "1"; break;
    }
    ui->stopbits->setCurrentText(stopBitsStr);

    // ������ʾ��ʽ
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
    // ��鲨�����Ƿ���ڣ������������
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

    // ��������
    saveINI();

    accept();
}