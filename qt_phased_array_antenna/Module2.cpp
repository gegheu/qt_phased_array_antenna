#include "Module2.h"
#include "ui_module2.h"
#include "SerialPort.h"
#include "Tcp.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCodec>

Module2::Module2(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Module2),
    m_serialPort(nullptr),
    m_tcp(nullptr),
    m_manager(nullptr),
    m_serialBaudRate(9600),
    m_serialDataBits(QSerialPort::Data8),
    m_serialParity(QSerialPort::NoParity),
    m_serialStopBits(QSerialPort::OneStop),
    m_serialHexDisplay(true),
    m_tcpIp("192.168.1.100"),
    m_tcpPort(8080),
    m_tcpHexDisplay(true),
    m_serialTxNum(0),
    m_serialRxNum(0),
    m_tcpConnectionState(TcpDisconnected)
{
    ui->setupUi(this);
   

    initSerial();
    initTcp();

    // 设置文本编码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

Module2::~Module2()
{
    delete ui;
    if (m_serialPort) {
        delete m_serialPort;
    }
    if (m_tcp) {
        delete m_tcp;
    }
    if (m_manager) {
        delete m_manager;
    }
}

void Module2::initSerial()
{
    m_serialPort = new SerialPort("Module2_Serial", this); // 设置唯一实例ID

    connect(m_serialPort, &ICommunication::dataReceived, this, &Module2::handleSerialDataReceived);
    connect(m_serialPort, &ICommunication::connectStatus, this, &Module2::handleOpenSerialResult);
}

void Module2::initTcp()
{
    m_tcp = new Tcp("Module2_TCP", this); // 设置唯一实例ID

    connect(m_tcp, &ICommunication::dataReceived, this, &Module2::handleTcpDataReceived);
    connect(m_tcp, &ICommunication::connectStatus, this, &Module2::handleTcpConnectedResult);
}

// 串口配置按钮点击
void Module2::on_serialConfigButton_clicked()
{
    SerialConfigDialog dialog(this);
    dialog.setCurrentConfig(m_serialPortName, m_serialBaudRate,
        m_serialDataBits, m_serialParity,
        m_serialStopBits, m_serialHexDisplay);

    if (dialog.exec() == QDialog::Accepted) {
        m_serialPortName = dialog.portName();
        m_serialBaudRate = dialog.baudRate();
        m_serialDataBits = dialog.dataBits();
        m_serialParity = dialog.parity();
        m_serialStopBits = dialog.stopBits();
        m_serialHexDisplay = dialog.hexDisplay();
    }
}

// 串口开关按钮点击
void Module2::on_serialSwitch_clicked()
{
    if (ui->serialSwitch->text() == "打开串口") {
        m_serialPort->portConnect(getSerialParaList());
        ui->serialConfigButton->setEnabled(false);
    }
    else {
        m_serialPort->disconnect();
        ui->serialConfigButton->setEnabled(true);
        ui->serialSwitch->setText(QStringLiteral("打开串口"));
    }
}

// 串口连接结果处理
void Module2::handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr)
{
    if (result) {
        ui->serialSwitch->setText(QStringLiteral("关闭串口"));
    }
    else {
        QMessageBox::warning(this, "警告", "打开串口失败: " + errStr);
        ui->serialConfigButton->setEnabled(true);
        ui->serialSwitch->setText(QStringLiteral("打开串口"));
    }
}

// 串口发送按钮点击
void Module2::on_serialSendButton_clicked()
{
    qint32 send_byte_size = 0;
    QString displayStr;
    QByteArray write_buff;
    QString data_buff = ui->serialSendDataDisplay->toPlainText();

    if (!m_serialPort->isConnected()) {
        QMessageBox::information(this, "提示", "请先打开串口");
        return;
    }

    if (m_serialHexDisplay) {
        QStringList hexBytes = data_buff.split(' ', Qt::SkipEmptyParts);
        for (auto byteStr : hexBytes) {
            bool ok;
            quint8 byte = byteStr.toUInt(&ok, 16);
            if (ok) {
                write_buff.append(byte);
                displayStr += QString("%1 ").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
            }
        }
    }
    else {
        write_buff.append(data_buff.toUtf8());
        displayStr = data_buff;
    }

    send_byte_size = m_serialPort->write(write_buff);
    m_serialTxNum += send_byte_size;
    ui->serialTxDisplay->setText("tx:" + QString::number(m_serialTxNum));
    appendSerialLog(displayStr, true);
}

// 串口数据接收处理
void Module2::handleSerialDataReceived(const QString& instanceId, const QByteArray& data)
{
    if (!data.isEmpty()) {
        QString result;
        if (m_serialHexDisplay) {
            for (quint8 byte : data) {
                result += QString("%1 ")
                    .arg(byte, 2, 16, QLatin1Char('0'))
                    .toUpper();
            }
        }
        else {
            result = QString(data);
        }
        m_serialRxNum += data.size();
        ui->serialRxDisplay->setText("rx:" + QString::number(m_serialRxNum));
        appendSerialLog(result, false);
    }
}

// 串口日志添加
void Module2::appendSerialLog(const QString& str, bool isSend)
{
    QString append_str = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");

    if (isSend) {
        append_str += "  SEND";
        ui->serialLogDisplay->append(append_str);
        ui->serialLogDisplay->setTextColor(Qt::blue);
        ui->serialLogDisplay->append(str);
        ui->serialLogDisplay->setTextColor(Qt::black);
    }
    else {
        append_str += "  RECV";
        ui->serialLogDisplay->append(append_str);
        ui->serialLogDisplay->setTextColor(Qt::darkCyan);
        ui->serialLogDisplay->append(str);
        ui->serialLogDisplay->setTextColor(Qt::black);
    }
}

// 串口清空日志按钮点击
void Module2::on_serialClearLogButton_clicked()
{
    ui->serialLogDisplay->clear();
}

// 串口清空发送数据按钮点击
void Module2::on_serialClearTxDataButton_clicked()
{
    ui->serialSendDataDisplay->clear();
}

// 串口清空计数按钮点击
void Module2::on_serialClearCountButton_clicked()
{
    ui->serialTxDisplay->clear();
    ui->serialRxDisplay->clear();
    m_serialRxNum = 0;
    m_serialTxNum = 0;
}

// TCP配置按钮点击
void Module2::on_tcpConfigButton_clicked()
{
    TcpConfigDialog dialog(this);
    dialog.setCurrentConfig(m_tcpIp, m_tcpPort, m_tcpHexDisplay);

    if (dialog.exec() == QDialog::Accepted) {
        m_tcpIp = dialog.ipAddress();
        m_tcpPort = dialog.port();
        m_tcpHexDisplay = dialog.hexDisplay();
    }
}

// TCP连接按钮点击
void Module2::on_tcpConnectButton_clicked()
{
    Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
    if (m_tcp->isConnected() || (tcp && tcp->isConnecting())) {
        return;
    }

    if (!m_tcp->isConnected()) {
        // 验证输入
        if (m_tcpIp.isEmpty()) {
            QMessageBox::warning(this, "输入错误", "请输入服务器IP地址");
            return;
        }

        if (m_tcpPort < 1 || m_tcpPort > 65535) {
            QMessageBox::warning(this, "端口错误", "端口号必须在1-65535之间");
            return;
        }

        // 更新连接状态
        m_tcpConnectionState = TcpConnecting;

        // 尝试连接服务器
        m_tcp->portConnect(getTcpParaList());
        ui->tcpLogDisplay->append(QStringLiteral("正在连接服务器: ") + m_tcpIp + ":" + QString::number(m_tcpPort));
        ui->tcpConnectButton->setEnabled(false);
        ui->tcpDisconnectButton->setEnabled(true);
        ui->tcpDisconnectButton->setText(QStringLiteral("取消连接"));
    }
}

// TCP断开连接按钮点击
void Module2::on_tcpDisconnectButton_clicked()
{
    Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
    if (!tcp) return;

    // 根据当前连接状态执行不同的断开操作
    if (m_tcpConnectionState == TcpConnecting) {
        // 正在连接中，中止连接尝试
        tcp->abortConnection();
        ui->tcpLogDisplay->append(QStringLiteral("连接已取消"));
    }
    else if (m_tcpConnectionState == TcpConnected) {
        // 已连接，正常断开
        tcp->disconnect();
        ui->tcpLogDisplay->append(QStringLiteral("连接已断开"));
    }

    // 更新UI状态
    ui->tcpConnectButton->setEnabled(true);
    ui->tcpDisconnectButton->setEnabled(false);
    ui->tcpDisconnectButton->setText(QStringLiteral("断开连接"));

    // 更新连接状态
    m_tcpConnectionState = TcpDisconnected;
}

// TCP发送按钮点击
void Module2::on_tcpSendButton_clicked()
{
    if (m_tcp->isConnected()) {
        QString data = ui->tcpSendDataEdit->text();
        if (!data.isEmpty()) {
            QByteArray byteArray;
            if (m_tcpHexDisplay) {
                // 十六进制发送
                QStringList hexList = data.split(' ', Qt::SkipEmptyParts);
                for (const QString& hexStr : hexList) {
                    bool ok;
                    quint8 byte = hexStr.toUInt(&ok, 16);
                    if (ok) {
                        byteArray.append(byte);
                    }
                }
                m_tcp->write(byteArray);
                ui->tcpLogDisplay->append(QStringLiteral("[发送] 十六进制: ") + byteArray.toHex(' ').toUpper());
            }
            else {
                // ASCII发送
                byteArray = data.toUtf8();
                m_tcp->write(byteArray);
                ui->tcpLogDisplay->append(QStringLiteral("[发送] 文本: ") + data);
            }
        }
    }
    else {
        ui->tcpLogDisplay->append(QStringLiteral("未连接服务器，无法发送数据"));
    }
}

// TCP数据接收处理
void Module2::handleTcpDataReceived(const QString& instanceId, const QByteArray& data)
{
    if (m_tcpHexDisplay) {
        // 显示十六进制格式
        QString hexData = data.toHex(' ').toUpper();
        ui->tcpLogDisplay->append(QStringLiteral("[接收] 十六进制: ") + hexData);
    }
    else {
        // 尝试显示文本（如果是文本数据）
        QString textData = QString::fromUtf8(data);
        ui->tcpLogDisplay->append(QStringLiteral("[接收] 文本: ") + textData);
    }
}

// TCP连接结果处理
void Module2::handleTcpConnectedResult(const QString& instanceId, bool result, const QString& errorInfo)
{
    if (result) {
        m_tcpConnectionState = TcpConnected;
        ui->tcpConnectButton->setText(QStringLiteral("已连接"));
        ui->tcpSendButton->setEnabled(true);
        ui->tcpLogDisplay->append(QStringLiteral("连接服务器成功!"));
        ui->tcpDisconnectButton->setEnabled(true);
        ui->tcpDisconnectButton->setText(QStringLiteral("断开连接"));
    }
    else {
        m_tcpConnectionState = TcpDisconnected;

        if (!errorInfo.isEmpty()) {
            QString displayError;

            if (errorInfo == "disconnected" ||
                errorInfo == "Connection timed out" ||
                errorInfo == "连接已取消" ||
                errorInfo == "手动断开") {
                if (errorInfo == "disconnected") {
                    displayError = "连接已断开";
                }
                else if (errorInfo == "Connection timed out") {
                    displayError = "连接超时";
                }
                else {
                    displayError = errorInfo;
                }
            }
            else {
                displayError = errorInfo;
                if (displayError.contains(QRegExp("[^\\x00-\\x7F]")) &&
                    (displayError.contains("?") || displayError.contains("�"))) {
                    QTextCodec* codec = QTextCodec::codecForLocale();
                    QByteArray encodedString = errorInfo.toLocal8Bit();
                    displayError = codec->toUnicode(encodedString);
                    if (displayError.isEmpty() || displayError.contains("?") || displayError.contains("�")) {
                        displayError = "连接错误";
                    }
                }
            }

            ui->tcpLogDisplay->append(QStringLiteral("连接错误: ") + displayError);
        }

        ui->tcpConnectButton->setText(QStringLiteral("连接服务器"));
        ui->tcpSendButton->setEnabled(false);
        ui->tcpDisconnectButton->setEnabled(false);
        ui->tcpDisconnectButton->setText(QStringLiteral("断开连接"));
    }

    ui->tcpConnectButton->setEnabled(true);
}

// TCP清空日志按钮点击
void Module2::on_tcpClearLogButton_clicked()
{
    ui->tcpLogDisplay->clear();
}

// TCP清空发送数据按钮点击
void Module2::on_tcpClearTxDataButton_clicked()
{
    ui->tcpSendDataEdit->clear();
}

// 获取串口参数列表
QVariantList Module2::getSerialParaList()
{
    QVariantList paraList;
    paraList.append(m_serialPortName);
    paraList.append(m_serialBaudRate);
    paraList.append(static_cast<int>(m_serialDataBits));
    paraList.append(static_cast<int>(m_serialParity));
    paraList.append(static_cast<int>(m_serialStopBits));
    return paraList;
}

// 获取TCP参数列表
QVariantList Module2::getTcpParaList()
{
    QVariantList paraList;
    paraList.append(m_tcpIp);
    paraList.append(m_tcpPort);
    return paraList;
}

// 协议测试函数
void Module2::test(VFProtocol::VFModuleFrame data)
{
    ui->serialLogDisplay->append("RECV:");
    ui->serialLogDisplay->append("VFStruct.frameHead=" + QString::number(data.frameHead, 16));
    ui->serialLogDisplay->append("VFStruct.cmd=" + QString::number(data.cmd, 16));
    ui->serialLogDisplay->append("VFStruct.frameLength=" + QString::number(data.frameLength, 16));
    ui->serialLogDisplay->append("VFStruct.data=" + data.data.toHex());
}