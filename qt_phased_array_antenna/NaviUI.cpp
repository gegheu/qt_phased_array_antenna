#include "NaviUI.h"
#include <QMessageBox>

NaviUI::NaviUI(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NaviReceiver)
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

NaviUI::~NaviUI()
{
    delete ui;
}

void NaviUI::setDevice(ICommunication* device)
{
    m_serialPort = device;
    if (m_serialPort) {
        connect(m_serialPort, &ICommunication::connectStatus,
            this, &NaviUI::handleOpenSerialResult);
    }
}

void NaviUI::on_set_uart_clicked()
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

void NaviUI::on_open_uart_clicked()
{
    if (!m_serialPort) return;

    if (ui->open_uart->text() == QStringLiteral("打开串口")) {
        if (m_serialPortName.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先配置串口"));
            return;
        }
        m_serialPort->portConnect(getSerialParaList());
    }
    else {
        m_serialPort->disconnect();
        ui->open_uart->setText(QStringLiteral("打开串口"));
        ui->set_uart->setEnabled(true);
    }
}

void NaviUI::handleOpenSerialResult(const QString&, bool result, const QString& errStr)
{
    if (result) {
        ui->open_uart->setText(QStringLiteral("关闭串口"));
        ui->set_uart->setEnabled(false);
    }
    else {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("打开串口失败: ") + errStr);
    }
}

QVariantList NaviUI::getSerialParaList() const
{
    return { m_serialPortName, m_serialBaudRate, (int)m_serialDataBits, (int)m_serialParity, (int)m_serialStopBits };
}
