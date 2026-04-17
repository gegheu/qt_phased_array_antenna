#include "powerCtrl.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCodec>

powerCtrl::powerCtrl(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::powerCtrlClass)
    , m_serialPort(nullptr)
    , m_protocol(nullptr)
    , m_serialConfigDialog(nullptr)
    , m_serialPortName("")
    , m_serialBaudRate(115200)
    , m_serialDataBits(QSerialPort::Data8)
    , m_serialParity(QSerialPort::NoParity)
    , m_serialStopBits(QSerialPort::OneStop)
{
    ui->setupUi(this);

    m_serialConfigDialog = new SerialConfigDialog(this);
    setWindowTitle(QStringLiteral("相控阵电源控制"));
}

powerCtrl::~powerCtrl()
{
    // 不再主动 disconnect，因为生命周期由外部管理
    delete ui;
}

// ==================== 资源注入 ====================

void powerCtrl::setDevice(ICommunication* device)
{
    m_serialPort = device;
    if (m_serialPort) {
        // 连接串口打开/关闭的状态反馈信号
        connect(m_serialPort, &ICommunication::connectStatus,
            this, &powerCtrl::handleOpenSerialResult);
    }
}

void powerCtrl::setProtocol(PowerProtocol* proto)
{
    m_protocol = proto;
    if (m_protocol) {
        // 连接协议解析完成后的业务处理信号
        connect(m_protocol, &PowerProtocol::powerStatusReceived,
            this, &powerCtrl::handlePowerStatus);
    }
}

// ==================== 串口控制逻辑 ====================

void powerCtrl::on_set_uart_clicked()
{
    if (m_serialConfigDialog) {
        m_serialConfigDialog->setCurrentConfig(m_serialPortName, m_serialBaudRate,
            m_serialDataBits, m_serialParity,
            m_serialStopBits, true);

        if (m_serialConfigDialog->exec() == QDialog::Accepted) {
            m_serialPortName = m_serialConfigDialog->portName();
            m_serialBaudRate = m_serialConfigDialog->baudRate();
            m_serialDataBits = m_serialConfigDialog->dataBits();
            m_serialParity = m_serialConfigDialog->parity();
            m_serialStopBits = m_serialConfigDialog->stopBits();
        }
    }
}

void powerCtrl::on_open_uart_clicked()
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

void powerCtrl::handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr)
{
    if (result) {
        ui->open_uart->setText(QStringLiteral("关闭串口"));
        ui->set_uart->setEnabled(false);
    }
    else {
        QMessageBox::warning(this, QStringLiteral("警告"),
            QStringLiteral("打开串口失败: ") + errStr);
        ui->open_uart->setText(QStringLiteral("打开串口"));
        ui->set_uart->setEnabled(true);
    }
}

// ==================== 电源业务处理 ====================

void powerCtrl::handlePowerStatus(const PowerProtocol::PowerStatusFrame& status)
{
    // 更新6个通道的显示
    for (int i = 0; i < 6; i++) {
        double voltage = PowerProtocol::voltageToDouble(status.channels[i].voltage);
        double current = PowerProtocol::currentToDouble(status.channels[i].current);
        updateUIDisplay(i, voltage, current);
    }

    // 更新输入功率和温度
    double power = PowerProtocol::powerToDouble(status.inputPower);
    double temp = PowerProtocol::temperatureToDouble(status.temperature);

    ui->input_w->setText(QString::number(power, 'f', 0));
    ui->power_temp->setText(QString::number(temp, 'f', 1));
}

void powerCtrl::updateUIDisplay(int channel, double voltage, double current)
{
    switch (channel) {
    case CH_12V1:
        ui->v_12v1->setText(QString::number(voltage, 'f', 2));
        ui->a_12v1->setText(QString::number(current, 'f', 2));
        break;
    case CH_12V2:
        ui->v_12v2->setText(QString::number(voltage, 'f', 2));
        ui->a_12v2->setText(QString::number(current, 'f', 2));
        break;
    case CH_12V3:
        ui->v_12v3->setText(QString::number(voltage, 'f', 2));
        ui->a_12v3->setText(QString::number(current, 'f', 2));
        break;
    case CH_12V4:
        ui->v_12v4->setText(QString::number(voltage, 'f', 2));
        ui->a_12v4->setText(QString::number(current, 'f', 2));
        break;
    default:
        break;
    }
}

// ==================== 命令构建与发送 ====================

QByteArray powerCtrl::buildControlData(int channel, bool open)
{
    QByteArray data(6, 0x00);
    for (int i = 0; i < 6; i++) {
        data[i] = PowerProtocol::getChannelCloseValue(i);
    }
    if (open && channel >= 0 && channel < 6) {
        data[channel] = PowerProtocol::getChannelOpenValue(channel);
    }
    return data;
}

QByteArray powerCtrl::buildControlDataAll(bool open)
{
    QByteArray data(6, 0x00);
    for (int i = 0; i < 6; i++) {
        data[i] = open ? PowerProtocol::getChannelOpenValue(i) : PowerProtocol::getChannelCloseValue(i);
    }
    return data;
}


// ==================== 12V通道控制 ====================
void powerCtrl::on_open12v1_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V1, true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_close12v1_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V1, false));
    m_serialPort->write(cmd);
}

void powerCtrl::on_open12v2_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V2, true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_close12v2_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V2, false));
    m_serialPort->write(cmd);
}

void powerCtrl::on_open12v3_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V3, true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_close12v3_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V3, false));
    m_serialPort->write(cmd);
}

void powerCtrl::on_open12v4_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V4, true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_close12v4_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_12V4, false));
    m_serialPort->write(cmd);
}


// ==================== 24V通道控制 ====================

void powerCtrl::on_open24v1_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_24V1, true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_close24v1_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_24V1, false));
    m_serialPort->write(cmd);
}

void powerCtrl::on_open24v2_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_24V2, true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_close24v2_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlData(CH_24V2, false));
    m_serialPort->write(cmd);
}

// ==================== 全部控制 ====================

void powerCtrl::on_openall_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlDataAll(true));
    m_serialPort->write(cmd);
}

void powerCtrl::on_closeall_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }
    QByteArray cmd = m_protocol->buildCommand(buildControlDataAll(false));
    m_serialPort->write(cmd);
}

QVariantList powerCtrl::getSerialParaList()
{
    QVariantList params;
    params.append(m_serialPortName);
    params.append(m_serialBaudRate);
    params.append(static_cast<int>(m_serialDataBits));
    params.append(static_cast<int>(m_serialParity));
    params.append(static_cast<int>(m_serialStopBits));
    return params;
}