#include "variableFreq.h"
#include "SerialPort.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCodec>
#include <QTextCursor>

variableFreq::variableFreq(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::variableFreq)
    , m_serialPort(nullptr)
    , m_protocol(nullptr)
    , m_serialConfigDialog(nullptr)
    , m_serialBaudRate(115200)
    , m_serialDataBits(QSerialPort::Data8)
    , m_serialParity(QSerialPort::NoParity)
    , m_serialStopBits(QSerialPort::OneStop)
    , m_serialTxNum(0)
    , m_serialRxNum(0)
{
    ui->setupUi(this);

    m_serialConfigDialog = new SerialConfigDialog(this);

    initSerial();
    initProtocol();
    initComboBox();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    setWindowTitle(QStringLiteral("变频控制"));
}

variableFreq::~variableFreq()
{
    if (m_serialPort && m_serialPort->isConnected()) {
        m_serialPort->disconnect();
    }
    delete ui;
}

void variableFreq::initSerial()
{
    m_serialPort = new SerialPort("VariableFreq_Serial", this);

    connect(m_serialPort, &ICommunication::dataReceived,
        this, &variableFreq::handleSerialDataReceived);
    connect(m_serialPort, &ICommunication::connectStatus,
        this, &variableFreq::handleOpenSerialResult);
}

void variableFreq::initProtocol()
{
    m_protocol = new VFProtocol(this);
    connect(m_protocol, &VFProtocol::varFreqResponse,
        this, &variableFreq::handleVarFreqResponse);
}

void variableFreq::initComboBox()
{
    ui->serialSendDataCMD->addItem(QStringLiteral("接收频率设置"), VFProtocol::RxFreq);
    ui->serialSendDataCMD->addItem(QStringLiteral("发射频率设置"), VFProtocol::TxFreq);
    ui->serialSendDataCMD->setCurrentIndex(0);
}

// ==================== 构建命令 ====================

QByteArray variableFreq::buildVarFreqCommand(quint8 cmd, quint32 freqMHz)
{
    QByteArray data;
    data.append(static_cast<char>(cmd));

    quint16 freqRaw = static_cast<quint16>(freqMHz);
    data.append(static_cast<char>((freqRaw >> 8) & 0xFF));
    data.append(static_cast<char>(freqRaw & 0xFF));

    QVariantMap emptyPara;
    return m_protocol->buildCommand(data, emptyPara);
}

bool variableFreq::getAndValidateFrequency(quint8 cmd, quint32& freqMHz)
{
    QString freqText = ui->serialSendData->toPlainText().trimmed();
    if (freqText.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入频率值"));
        return false;
    }

    bool ok;
    freqMHz = freqText.toUInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("频率格式错误，请输入整数"));
        return false;
    }

    quint32 clampedFreq = freqMHz;
    QString rangeStr;

    if (cmd == VFProtocol::RxFreq) {
        if (!VFProtocol::isValidRxFrequency(freqMHz)) {
            clampedFreq = VFProtocol::clampRxFrequency(freqMHz);
            rangeStr = QStringLiteral("接收频率范围: %1~%2 MHz").arg(RX_FREQ_MIN).arg(RX_FREQ_MAX);
        }
    }
    else if (cmd == VFProtocol::TxFreq) {
        if (!VFProtocol::isValidTxFrequency(freqMHz)) {
            clampedFreq = VFProtocol::clampTxFrequency(freqMHz);
            rangeStr = QStringLiteral("发射频率范围: %1~%2 MHz").arg(TX_FREQ_MIN).arg(TX_FREQ_MAX);
        }
    }

    if (!rangeStr.isEmpty()) {
        freqMHz = clampedFreq;
        QMessageBox::warning(this, QStringLiteral("频率超出范围"),
            rangeStr + QStringLiteral("\n已调整为最接近频率: %1 MHz").arg(freqMHz));
    }

    return true;
}

// ==================== 串口控制 ====================

void variableFreq::on_serialConfigButton_clicked()
{
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

void variableFreq::on_serialSwitch_clicked()
{
    if (ui->serialSwitch->text() == QStringLiteral("打开串口")) {
        if (m_serialPortName.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请先配置串口"));
            return;
        }
        m_serialPort->portConnect(getSerialParaList());
    }
    else {
        m_serialPort->disconnect();
        ui->serialSwitch->setText(QStringLiteral("打开串口"));
        ui->serialConfigButton->setEnabled(true);
    }
}

void variableFreq::handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr)
{
    if (result) {
        ui->serialSwitch->setText(QStringLiteral("关闭串口"));
        ui->serialConfigButton->setEnabled(false);
        appendSerialLog(QStringLiteral("串口打开成功"), false);
    }
    else {
        QMessageBox::warning(this, QStringLiteral("警告"),
            QStringLiteral("打开串口失败: ") + errStr);
        ui->serialSwitch->setText(QStringLiteral("打开串口"));
        ui->serialConfigButton->setEnabled(true);
    }
}

void variableFreq::handleSerialDataReceived(const QString& instanceId, const QByteArray& data)
{
    if (m_protocol) {
        m_protocol->parseResponse(data);
    }

    QString hexData = data.toHex(' ').toUpper();
    appendSerialLog(hexData, false);
    updateSerialCounters(0, data.size());
}

void variableFreq::on_serialSendButton_clicked()
{
    if (!m_serialPort || !m_serialPort->isConnected()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先打开串口"));
        return;
    }

    quint8 cmd = static_cast<quint8>(ui->serialSendDataCMD->currentData().toInt());

    quint32 freqMHz;
    if (!getAndValidateFrequency(cmd, freqMHz)) {
        return;
    }

    QByteArray command = buildVarFreqCommand(cmd, freqMHz);

    QString cmdName = ui->serialSendDataCMD->currentText();
    appendSerialLog(QStringLiteral("发送 [%1] 频率: %2 MHz").arg(cmdName).arg(freqMHz), true);
    appendSerialLog(QStringLiteral("原始数据: ") + command.toHex(' ').toUpper(), true);

    qint64 sent = m_serialPort->write(command);
    updateSerialCounters(sent, 0);
}

void variableFreq::appendSerialLog(const QString& str, bool isSend)
{
    QString timestamp = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");

    if (isSend) {
        ui->serialLogDisplay->setTextColor(Qt::blue);
        ui->serialLogDisplay->append(timestamp + " SEND: " + str);
    }
    else {
        ui->serialLogDisplay->setTextColor(Qt::darkCyan);
        ui->serialLogDisplay->append(timestamp + " RECV: " + str);
    }
    ui->serialLogDisplay->setTextColor(Qt::black);
    ui->serialLogDisplay->moveCursor(QTextCursor::End);
}

void variableFreq::updateSerialCounters(qint64 sent, qint64 received)
{
    m_serialTxNum += sent;
    m_serialRxNum += received;
    ui->serialTxDisplay->setText(QString::number(m_serialTxNum));
    ui->serialRxDisplay->setText(QString::number(m_serialRxNum));
}

void variableFreq::on_serialClearLogButton_clicked()
{
    ui->serialLogDisplay->clear();
}

void variableFreq::on_serialClearCountButton_clicked()
{
    m_serialTxNum = 0;
    m_serialRxNum = 0;
    ui->serialTxDisplay->setText("0");
    ui->serialRxDisplay->setText("0");
}

// ==================== 协议响应处理 ====================

void variableFreq::handleVarFreqResponse(const VFProtocol::VarFreqResponse& response)
{
    if (response.isValid) {
        if (response.cmd == VFProtocol::RxFreq) {
            appendSerialLog(QStringLiteral("接收频率设置成功"), false);
        }
        else if (response.cmd == VFProtocol::TxFreq) {
            appendSerialLog(QStringLiteral("发射频率设置成功"), false);
        }
        else {
            appendSerialLog(QStringLiteral("收到响应: 命令=0x%1").arg(response.cmd, 2, 16, QLatin1Char('0')), false);
        }
    }
    else {
        appendSerialLog(QStringLiteral("收到无效响应"), false);
    }
}

// ==================== 获取参数列表 ====================

QVariantList variableFreq::getSerialParaList()
{
    QVariantList params;
    params.append(m_serialPortName);
    params.append(m_serialBaudRate);
    params.append(static_cast<int>(m_serialDataBits));
    params.append(static_cast<int>(m_serialParity));
    params.append(static_cast<int>(m_serialStopBits));
    return params;
}