#include "AnteCtrl.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCodec>
#include <QTextCursor>

AnteCtrl::AnteCtrl(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AnteCtrlClass)
    , m_serialPort(nullptr)
    , m_protocol(nullptr)
    , m_serialConfigDialog(nullptr)
    , m_serialBaudRate(115200)
    , m_serialDataBits(QSerialPort::Data8)
    , m_serialParity(QSerialPort::NoParity)
    , m_serialStopBits(QSerialPort::OneStop)
    , m_txNum(0)
    , m_rxNum(0)
    , m_autoScroll(true)
    , m_maxLogLines(10000)
{
    ui->setupUi(this);

    // 初始化控件默认值
    ui->beam_sel->addItems({ "1", "2", "3", "4", "5", "6", "7", "8" });
    ui->subarray_sel->addItems({
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "11", "12", "13", "14" });
    ui->send_ante_Button->setChecked(true);

    // 设置日志控件样式
    ui->ante_recv_log->setReadOnly(true);
    QFont logFont("Consolas", 9);
    ui->ante_recv_log->setFont(logFont);

    m_serialConfigDialog = new SerialConfigDialog(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

AnteCtrl::~AnteCtrl()
{
    delete ui;
}

// ==================== 资源注入 ====================

void AnteCtrl::setDevice(ICommunication* device)
{
    m_serialPort = device;
    if (m_serialPort) {
        // 连接状态反馈信号
        connect(m_serialPort, &ICommunication::connectStatus,
            this, &AnteCtrl::handleOpenSerialResult);

        // 原始数据日志连接：当串口收到数据时，自动打印 16 进制日志
        connect(m_serialPort, &ICommunication::dataReceived, this, [this](const QString&, const QByteArray& data) {
            appendLog(data.toHex(' ').toUpper(), false);
            m_rxNum += data.size();
            });
    }
}

void AnteCtrl::setProtocol(AntennaProtocol* proto)
{
    m_protocol = proto;
    if (m_protocol) {
        // 当管理器的协议解析完数据后，触发此处的 UI 更新
        connect(m_protocol, &AntennaProtocol::AnteEvent,
            this, &AnteCtrl::onDataUpdated);
    }
}

// ==================== 数据显示 ====================

void AnteCtrl::onDataUpdated(const AntennaProtocol::AnteModuleFrame& frame)
{
    // 原 handleAntennaEvent 逻辑：根据指令码更新 UI 状态显示
    QString cmdType;
    switch (frame.cmd) {
    case 0x01: cmdType = QStringLiteral("射频通道开关控制"); break;
    case 0x02: cmdType = QStringLiteral("单通道移相衰减控制"); break;
    case 0x03: cmdType = QStringLiteral("校准值写入"); break;
    case 0x04: cmdType = QStringLiteral("校准值读出"); break;
    case 0x05: cmdType = QStringLiteral("控制码写入"); break;
    case 0x06: cmdType = QStringLiteral("控制码读出"); break;
    case 0x07: cmdType = QStringLiteral("加权码写入"); break;
    case 0x08: cmdType = QStringLiteral("加权码读出"); break;
    case 0x09: cmdType = QStringLiteral("波束指向控制"); break;
    default: cmdType = QStringLiteral("未知指令");
    }
    ui->ante_recv_type->setText(cmdType);
}

// ==================== 串口控制 ====================

void AnteCtrl::on_set_uart_clicked()
{
    if (m_serialConfigDialog) {
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
}

void AnteCtrl::on_open_uart_clicked()
{
    if (!m_serialPort) return;

    if (ui->open_uart->text() == QStringLiteral("打开串口")) {
        if (m_serialPortName.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先配置串口"));
            return;
        }
        m_serialPort->portConnect(getSerialParaList()); // 通过注入的接口连接
    }
    else {
        m_serialPort->disconnect(); // 通过基类接口断开
        ui->open_uart->setText(QStringLiteral("打开串口"));
        ui->set_uart->setEnabled(true);
    }
}

void AnteCtrl::handleOpenSerialResult(const QString& id, bool result, const QString& errStr)
{
    if (result) {
        ui->open_uart->setText(QStringLiteral("关闭串口"));
        ui->set_uart->setEnabled(false);
    }
    else {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("打开串口失败: ") + errStr);
    }
}

// ==================== 协议1: 射频通道开关控制指令 ====================
QByteArray AnteCtrl::buildSwitchCtrlCommand()
{
    QByteArray data;

    data.append(getAntennaSelect());
    data.append(static_cast<quint8>(ui->beam_sel->currentIndex() + 1));
    data.append(static_cast<quint8>(ui->subarray_sel->currentIndex() + 1));

    quint16 position = static_cast<quint16>(ui->spkg_sptd->text().toUInt(nullptr, 16));
    data.append(static_cast<quint8>((position >> 8) & 0xFF));
    data.append(static_cast<quint8>(position & 0xFF));

    quint8 action = static_cast<quint8>(ui->spkg_spkg->text().toUInt(nullptr, 16));
    data.append(action);

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x01);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_spkg_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildSwitchCtrlCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议2: 单通道移相衰减控制指令 ====================
QByteArray AnteCtrl::buildAttenuatorCtrlCommand()
{
    QByteArray data;

    data.append(getAntennaSelect());
    data.append(static_cast<quint8>(ui->beam_sel->currentIndex() + 1));
    data.append(static_cast<quint8>(ui->subarray_sel->currentIndex() + 1));

    quint16 position = static_cast<quint16>(ui->dtd_wz->text().toUInt(nullptr, 16));
    data.append(static_cast<quint8>((position >> 8) & 0xFF));
    data.append(static_cast<quint8>(position & 0xFF));

    quint16 code = static_cast<quint16>(ui->dtd_yxsjm->text().toUInt(nullptr, 16));
    data.append(static_cast<quint8>((code >> 8) & 0xFF));
    data.append(static_cast<quint8>(code & 0xFF));

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x02);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_dtdyx_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildAttenuatorCtrlCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议3: 移相衰减校准值写入 ====================
QByteArray AnteCtrl::buildCalibrationWriteCommand()
{
    QByteArray data;

    quint8 id = static_cast<quint8>(ui->yxsjjzz_write_id->text().toUInt(nullptr, 16));
    data.append(id);

    quint16 value = static_cast<quint16>(ui->yxsjjzz_write_num->text().toUInt(nullptr, 16));
    data.append(static_cast<quint8>((value >> 8) & 0xFF));
    data.append(static_cast<quint8>(value & 0xFF));

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x03);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_yxsjjzz_write_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildCalibrationWriteCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议4: 移相衰减校准值读出 ====================
QByteArray AnteCtrl::buildCalibrationReadCommand()
{
    QByteArray data;

    quint8 id = static_cast<quint8>(ui->yxsjjzz_read_id->text().toUInt(nullptr, 16));
    data.append(id);

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x04);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_yxsjjzz_read_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildCalibrationReadCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议5: 移相衰减控制码写入 ====================
QByteArray AnteCtrl::buildControlCodeWriteCommand()
{
    QByteArray data;

    quint16 code = static_cast<quint16>(ui->yxsjkzm_write_num->text().toUInt(nullptr, 16));
    data.append(static_cast<quint8>((code >> 8) & 0xFF));
    data.append(static_cast<quint8>(code & 0xFF));

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x05);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_yxsjkzm_write_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildControlCodeWriteCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议6: 移相衰减控制码读出 ====================
QByteArray AnteCtrl::buildControlCodeReadCommand()
{
    QByteArray data;
    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x06);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_yxsjkzm_read_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildControlCodeReadCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议7: 幅相加权控制码写入 ====================
QByteArray AnteCtrl::buildWeightCodeWriteCommand()
{
    QByteArray data;

    quint8 id = static_cast<quint8>(ui->fxjqkzm_write_id->text().toUInt(nullptr, 16));
    data.append(id);

    quint16 code = static_cast<quint16>(ui->fxjqkzm_write_num->text().toUInt(nullptr, 16));
    data.append(static_cast<quint8>((code >> 8) & 0xFF));
    data.append(static_cast<quint8>(code & 0xFF));

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x07);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_fxjqkzm_write_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildWeightCodeWriteCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议8: 幅相加权控制码读出 ====================
QByteArray AnteCtrl::buildWeightCodeReadCommand()
{
    QByteArray data;

    quint8 id = static_cast<quint8>(ui->fxjqkzm_read_id->text().toUInt(nullptr, 16));
    data.append(id);

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x08);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_fxjqkzm_read_send_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildWeightCodeReadCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

// ==================== 协议9: 波束指向控制指令 ====================
QByteArray AnteCtrl::buildBeamSteeringCommand()
{
    QByteArray data;

    quint16 freq = static_cast<quint16>(ui->bszx_pl->text().toUInt());
    data.append(static_cast<quint8>((freq >> 8) & 0xFF));
    data.append(static_cast<quint8>(freq & 0xFF));

    qint16 azimuth = static_cast<qint16>(ui->bszx_fwj->text().toInt());
    data.append(static_cast<quint8>((azimuth >> 8) & 0xFF));
    data.append(static_cast<quint8>(azimuth & 0xFF));

    qint16 elevation = static_cast<qint16>(ui->bszx_fyj->text().toInt());
    data.append(static_cast<quint8>((elevation >> 8) & 0xFF));
    data.append(static_cast<quint8>(elevation & 0xFF));

    QVariantMap params;
    params[ANTE_CMD] = static_cast<quint8>(0x09);

    return m_protocol->buildCommand(data, params);
}

void AnteCtrl::on_open_uart_11_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    QByteArray cmd = buildBeamSteeringCommand();
    QString hexStr = cmd.toHex(' ').toUpper();
    appendLog(hexStr, true);

    m_serialPort->write(cmd);
    m_txNum += cmd.size();
}

QVariantList AnteCtrl::getSerialParaList() {
    return { m_serialPortName, m_serialBaudRate, (int)m_serialDataBits, (int)m_serialParity, (int)m_serialStopBits };
}

void AnteCtrl::appendLog(const QString& str, bool isSend) {
    QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    ui->ante_recv_log->setTextColor(isSend ? Qt::blue : Qt::darkCyan);
    ui->ante_recv_log->append(timestamp + (isSend ? " SEND: " : " RECV: ") + str);
    ui->ante_recv_log->setTextColor(Qt::black);
    if (m_autoScroll) scrollToBottom();
}

void AnteCtrl::scrollToBottom() {
    QTextCursor cursor = ui->ante_recv_log->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->ante_recv_log->setTextCursor(cursor);
}

quint8 AnteCtrl::getAntennaSelect()
{
    return ui->send_ante_Button->isChecked() ? 0x01 : 0x02;
}

quint8 AnteCtrl::getSwitchAction()
{
    return 0x01;
}