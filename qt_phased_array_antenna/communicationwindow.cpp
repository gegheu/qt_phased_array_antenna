#include "communicationwindow.h"
#include "ui_communication_ui1.h"
#include "ui_communication_ui2.h"
#include "ui_communication_ui3.h"
#include "ui_communication_ui4.h"
#include "ui_communication_ui5.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>

CommunicationWindow::CommunicationWindow(int windowId, QWidget* parent)
    : QMainWindow(parent)
    , m_windowId(windowId)
    , m_tcpConnectionState(Disconnected)
{
    // 根据窗口ID加载不同的UI
    switch (windowId) {
    case 1:
        m_ui = new Ui::CommunicationClass1();
        static_cast<Ui::CommunicationClass1*>(m_ui)->setupUi(this);
        break;
    case 2:
        m_ui = new Ui::CommunicationClass2();
        static_cast<Ui::CommunicationClass2*>(m_ui)->setupUi(this);
        break;
    case 3:
        m_ui = new Ui::CommunicationClass3();
        static_cast<Ui::CommunicationClass3*>(m_ui)->setupUi(this);
        break;
    case 4:
        m_ui = new Ui::CommunicationClass4();
        static_cast<Ui::CommunicationClass4*>(m_ui)->setupUi(this);
        break;
    case 5:
        m_ui = new Ui::CommunicationClass5();
        static_cast<Ui::CommunicationClass5*>(m_ui)->setupUi(this);
        break;
    default:
        m_ui = new Ui::CommunicationClass1();
        static_cast<Ui::CommunicationClass1*>(m_ui)->setupUi(this);
        break;
    }

    // 设置窗口标题
    setWindowTitle(QString("通信窗口 %1").arg(windowId));

    // 初始化默认配置
    m_serialBaudRate = 9600;
    m_serialDataBits = QSerialPort::Data8;
    m_serialParity = QSerialPort::NoParity;
    m_serialStopBits = QSerialPort::OneStop;
    m_serialHexDisplay = true;

    m_tcpIp = "192.168.1.100";
    m_tcpPort = 8080;
    m_tcpHexDisplay = true;

    // 创建配置对话框
    m_serialConfigDialog = new SerialConfigDialog(this);
    m_tcpConfigDialog = new TcpConfigDialog(this);

    initSerial();
    initNetwork();
    setupConnections();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

CommunicationWindow::~CommunicationWindow()
{
    // 根据窗口ID删除对应的UI
    switch (m_windowId) {
    case 1:
        delete static_cast<Ui::CommunicationClass1*>(m_ui);
        break;
    case 2:
        delete static_cast<Ui::CommunicationClass2*>(m_ui);
        break;
    case 3:
        delete static_cast<Ui::CommunicationClass3*>(m_ui);
        break;
    case 4:
        delete static_cast<Ui::CommunicationClass4*>(m_ui);
        break;
    case 5:
        delete static_cast<Ui::CommunicationClass5*>(m_ui);
        break;
    default:
        delete static_cast<Ui::CommunicationClass1*>(m_ui);
        break;
    }
}

void CommunicationWindow::closeEvent(QCloseEvent* event)
{
    // 关闭串口和网络连接
    if (m_serialPort && m_serialPort->isConnected()) {
        m_serialPort->disconnect();
    }

    if (m_tcp && m_tcp->isConnected()) {
        Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
        if (tcp) {
            tcp->disconnect();
        }
    }

    emit windowClosed(m_windowId);
    event->accept();
}

void CommunicationWindow::setupConnections()
{
    // 这里需要根据每个窗口的UI控件名称进行连接
    // 由于每个窗口的UI可能不同，这里需要为每个窗口单独处理

    // 示例：为窗口1设置连接
    if (m_windowId == 1) {
        Ui::CommunicationClass1* ui = static_cast<Ui::CommunicationClass1*>(m_ui);

        // 连接串口相关信号槽
        connect(ui->serial_switch, &QPushButton::clicked, this, &CommunicationWindow::handleOpenSerial);
        connect(ui->send_button, &QPushButton::clicked, this, &CommunicationWindow::handleSendData);
        connect(ui->rxtx_clear, &QPushButton::clicked, this, &CommunicationWindow::handleClearTxRx);
        connect(ui->log_clear, &QPushButton::clicked, this, &CommunicationWindow::handleClearLog);
        connect(ui->tx_data_clear, &QPushButton::clicked, this, &CommunicationWindow::handleClearTxData);
        connect(ui->serialConfigButton, &QPushButton::clicked, this, &CommunicationWindow::onSerialConfigClicked);

        // 连接网口相关信号槽
        connect(ui->pushButtonConnect, &QPushButton::clicked, this, &CommunicationWindow::onConnectClicked);
        connect(ui->pushButtonSend, &QPushButton::clicked, this, &CommunicationWindow::onSendClicked);
        connect(ui->pushButtonDisConnect, &QPushButton::clicked, this, &CommunicationWindow::onDisconnectClicked);
        connect(ui->log_clear_tcp, &QPushButton::clicked, this, &CommunicationWindow::handleTcpClearLog);
        connect(ui->tx_data_clear_tcp, &QPushButton::clicked, this, &CommunicationWindow::handleTcpClearTxData);
        connect(ui->tcpConfigButton, &QPushButton::clicked, this, &CommunicationWindow::onTcpConfigClicked);
    }
    // 为其他窗口设置类似的连接...
    // 注意：每个窗口的控件名称可能不同，需要根据实际情况调整
}

void CommunicationWindow::test(VFProtocol::VFModuleFrame data)
{
    // 根据窗口ID获取日志显示控件
    QTextEdit* logDisplay = nullptr;
    switch (m_windowId) {
    case 1:
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->log_display;
        break;
    case 2:
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->log_display;
        break;
    case 3:
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->log_display;
        break;
    case 4:
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->log_display;
        break;
    case 5:
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->log_display;
        break;
    }

    if (logDisplay) {
        logDisplay->append("RECV:");
        logDisplay->append("VFStruct.frameHead=" + QString::number(data.frameHead, 16));
        logDisplay->append("VFStruct.cmd=" + QString::number(data.cmd, 16));
        logDisplay->append("VFStruct.frameLength=" + QString::number(data.frameLength, 16));
        logDisplay->append("VFStruct.data=" + data.data.toHex());
    }
}

void CommunicationWindow::onSerialConfigClicked()
{
    m_serialConfigDialog->setCurrentConfig(m_serialPortName, m_serialBaudRate,
        m_serialDataBits, m_serialParity,
        m_serialStopBits, m_serialHexDisplay);

    if (m_serialConfigDialog->exec() == QDialog::Accepted) {
        m_serialPortName = m_serialConfigDialog->portName();
        m_serialBaudRate = m_serialConfigDialog->baudRate();
        m_serialDataBits = m_serialConfigDialog->dataBits();
        m_serialParity = m_serialConfigDialog->parity();
        m_serialStopBits = m_serialConfigDialog->stopBits();
        m_serialHexDisplay = m_serialConfigDialog->hexDisplay();
    }
}

void CommunicationWindow::onTcpConfigClicked()
{
    m_tcpConfigDialog->setCurrentConfig(m_tcpIp, m_tcpPort, m_tcpHexDisplay);

    if (m_tcpConfigDialog->exec() == QDialog::Accepted) {
        m_tcpIp = m_tcpConfigDialog->ipAddress();
        m_tcpPort = m_tcpConfigDialog->port();
        m_tcpHexDisplay = m_tcpConfigDialog->hexDisplay();
    }
}

// 连接服务器
void CommunicationWindow::onConnectClicked()
{
    Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
    if (m_tcp->isConnected() || (tcp && tcp->isConnecting())) {
        return;
    }
    if (!m_tcp->isConnected()) {
        // 验证输入
        if (m_tcpIp.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("输入错误"), QStringLiteral("请输入服务器IP地址"));
            return;
        }

        if (m_tcpPort < 1 || m_tcpPort > 65535) {
            QMessageBox::warning(this, QStringLiteral("端口错误"), QStringLiteral("端口号必须在1-65535之间"));
            return;
        }

        // 更新连接状态
        m_tcpConnectionState = Connecting;

        // 尝试连接服务器
        m_tcp->portConnect(getTcpParaList());

        // 根据窗口ID获取日志显示控件
        QTextEdit* logDisplay = nullptr;
        switch (m_windowId) {
        case 1:
            logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
            break;
        case 2:
            logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
            break;
        case 3:
            logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
            break;
        case 4:
            logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
            break;
        case 5:
            logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
            break;
        }

        if (logDisplay) {
            logDisplay->append(QStringLiteral("正在连接服务器: ") + m_tcpIp + ":" + QString::number(m_tcpPort));
        }

        // 禁用连接按钮直到连接完成
        QPushButton* connectButton = nullptr;
        QPushButton* disconnectButton = nullptr;

        switch (m_windowId) {
        case 1:
            connectButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonConnect;
            disconnectButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonDisConnect;
            break;
        case 2:
            connectButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonConnect;
            disconnectButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonDisConnect;
            break;
        case 3:
            connectButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonConnect;
            disconnectButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonDisConnect;
            break;
        case 4:
            connectButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonConnect;
            disconnectButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonDisConnect;
            break;
        case 5:
            connectButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonConnect;
            disconnectButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonDisConnect;
            break;
        }

        if (connectButton) connectButton->setEnabled(false);
        if (disconnectButton) {
            disconnectButton->setEnabled(true);
            disconnectButton->setText(QStringLiteral("取消连接"));
        }
    }
}

// 发送数据
void CommunicationWindow::onSendClicked()
{
    if (m_tcp->isConnected()) {
        // 根据窗口ID获取发送数据控件
        QLineEdit* sendEdit = nullptr;
        QTextEdit* logDisplay = nullptr;

        switch (m_windowId) {
        case 1:
            sendEdit = static_cast<Ui::CommunicationClass1*>(m_ui)->lineEditTextToSend;
            logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
            break;
        case 2:
            sendEdit = static_cast<Ui::CommunicationClass2*>(m_ui)->lineEditTextToSend;
            logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
            break;
        case 3:
            sendEdit = static_cast<Ui::CommunicationClass3*>(m_ui)->lineEditTextToSend;
            logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
            break;
        case 4:
            sendEdit = static_cast<Ui::CommunicationClass4*>(m_ui)->lineEditTextToSend;
            logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
            break;
        case 5:
            sendEdit = static_cast<Ui::CommunicationClass5*>(m_ui)->lineEditTextToSend;
            logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
            break;
        }

        if (!sendEdit || !logDisplay) return;

        QString data = sendEdit->text();
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
                logDisplay->append(QStringLiteral("[发送] 十六进制: ") + byteArray.toHex(' ').toUpper());
            }
            else {
                // ASCII发送
                byteArray = data.toUtf8();
                m_tcp->write(byteArray);
                logDisplay->append(QStringLiteral("[发送] 文本: ") + data);
            }
        }
    }
    else {
        // 根据窗口ID获取日志显示控件
        QTextEdit* logDisplay = nullptr;
        switch (m_windowId) {
        case 1:
            logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
            break;
        case 2:
            logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
            break;
        case 3:
            logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
            break;
        case 4:
            logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
            break;
        case 5:
            logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
            break;
        }

        if (logDisplay) {
            logDisplay->append(QStringLiteral("未连接服务器，无法发送数据"));
        }
    }
}

// 断开连接
void CommunicationWindow::onDisconnectClicked()
{
    Tcp* tcp = qobject_cast<Tcp*>(m_tcp);
    if (!tcp) return;

    // 根据当前连接状态执行不同的断开操作
    if (m_tcpConnectionState == Connecting) {
        // 正在连接中，中止连接尝试
        tcp->abortConnection();

        // 根据窗口ID获取日志显示控件
        QTextEdit* logDisplay = nullptr;
        switch (m_windowId) {
        case 1:
            logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
            break;
        case 2:
            logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
            break;
        case 3:
            logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
            break;
        case 4:
            logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
            break;
        case 5:
            logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
            break;
        }

        if (logDisplay) {
            logDisplay->append(QStringLiteral("连接已取消"));
        }
    }
    else if (m_tcpConnectionState == Connected) {
        // 已连接，正常断开
        tcp->disconnect();

        // 根据窗口ID获取日志显示控件
        QTextEdit* logDisplay = nullptr;
        switch (m_windowId) {
        case 1:
            logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
            break;
        case 2:
            logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
            break;
        case 3:
            logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
            break;
        case 4:
            logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
            break;
        case 5:
            logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
            break;
        }

        if (logDisplay) {
            logDisplay->append(QStringLiteral("连接已断开"));
        }
    }

    // 更新UI状态
    QPushButton* connectButton = nullptr;
    QPushButton* disconnectButton = nullptr;

    switch (m_windowId) {
    case 1:
        connectButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonDisConnect;
        break;
    case 2:
        connectButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonDisConnect;
        break;
    case 3:
        connectButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonDisConnect;
        break;
    case 4:
        connectButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonDisConnect;
        break;
    case 5:
        connectButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonDisConnect;
        break;
    }

    if (connectButton) connectButton->setEnabled(true);
    if (disconnectButton) {
        disconnectButton->setEnabled(false);
        disconnectButton->setText(QStringLiteral("断开连接"));
    }

    // 更新连接状态
    m_tcpConnectionState = Disconnected;
}

// 接收数据
void CommunicationWindow::onDataReceived(const QByteArray& data, bool hexDisplay)
{
    // 根据窗口ID获取日志显示控件
    QTextEdit* logDisplay = nullptr;
    switch (m_windowId) {
    case 1:
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
        break;
    case 2:
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
        break;
    case 3:
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
        break;
    case 4:
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
        break;
    case 5:
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
        break;
    }

    if (!logDisplay) return;

    if (hexDisplay)
    {
        // 显示十六进制格式
        QString hexData = data.toHex(' ').toUpper();
        logDisplay->append(QStringLiteral("[接收] 十六进制: ") + hexData);
    }
    else
    {
        // 尝试显示文本（如果是文本数据）
        QString textData = QString::fromUtf8(data);
        logDisplay->append(QStringLiteral("[接收] 文本: ") + textData);
    }
}

// 错误处理
void CommunicationWindow::onConnectedResult(bool result, const QString& errorInfo)
{
    // 根据窗口ID获取UI控件
    QPushButton* connectButton = nullptr;
    QPushButton* disconnectButton = nullptr;
    QPushButton* sendButton = nullptr;
    QTextEdit* logDisplay = nullptr;

    switch (m_windowId) {
    case 1:
        connectButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonDisConnect;
        sendButton = static_cast<Ui::CommunicationClass1*>(m_ui)->pushButtonSend;
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
        break;
    case 2:
        connectButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonDisConnect;
        sendButton = static_cast<Ui::CommunicationClass2*>(m_ui)->pushButtonSend;
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
        break;
    case 3:
        connectButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonDisConnect;
        sendButton = static_cast<Ui::CommunicationClass3*>(m_ui)->pushButtonSend;
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
        break;
    case 4:
        connectButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonDisConnect;
        sendButton = static_cast<Ui::CommunicationClass4*>(m_ui)->pushButtonSend;
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
        break;
    case 5:
        connectButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonConnect;
        disconnectButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonDisConnect;
        sendButton = static_cast<Ui::CommunicationClass5*>(m_ui)->pushButtonSend;
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
        break;
    }

    if (!connectButton || !disconnectButton || !sendButton || !logDisplay) return;

    if (result) {
        m_tcpConnectionState = Connected;
        connectButton->setText(QStringLiteral("已连接"));
        sendButton->setEnabled(true);
        logDisplay->append(QStringLiteral("连接服务器成功!"));
        disconnectButton->setEnabled(true);
        disconnectButton->setText(QStringLiteral("断开连接"));
    }
    else {
        m_tcpConnectionState = Disconnected;

        if (!errorInfo.isEmpty()) {
            QString displayError;

            if (errorInfo == "disconnected" ||
                errorInfo == "Connection timed out" ||
                errorInfo == "连接已取消" ||
                errorInfo == "手动断开") {
                if (errorInfo == "disconnected") {
                    displayError = QStringLiteral("连接已断开");
                }
                else if (errorInfo == "Connection timed out") {
                    displayError = QStringLiteral("连接超时");
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
                        displayError = QStringLiteral("连接错误");
                    }
                }
            }

            logDisplay->append(QStringLiteral("连接错误: ") + displayError);
        }

        connectButton->setText(QStringLiteral("连接服务器"));
        sendButton->setEnabled(false);
        disconnectButton->setEnabled(false);
        disconnectButton->setText(QStringLiteral("断开连接"));
    }

    connectButton->setEnabled(true);
}

void CommunicationWindow::initNetwork()
{
    m_tcp = new Tcp(this);
}

void CommunicationWindow::handleTcpClearLog()
{
    // 根据窗口ID获取日志显示控件
    QTextEdit* logDisplay = nullptr;
    switch (m_windowId) {
    case 1:
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->textEditLog;
        break;
    case 2:
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->textEditLog;
        break;
    case 3:
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->textEditLog;
        break;
    case 4:
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->textEditLog;
        break;
    case 5:
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->textEditLog;
        break;
    }

    if (logDisplay) {
        logDisplay->clear();
    }
}

void CommunicationWindow::handleTcpClearTxData()
{
    // 根据窗口ID获取发送数据控件
    QLineEdit* sendEdit = nullptr;
    switch (m_windowId) {
    case 1:
        sendEdit = static_cast<Ui::CommunicationClass1*>(m_ui)->lineEditTextToSend;
        break;
    case 2:
        sendEdit = static_cast<Ui::CommunicationClass2*>(m_ui)->lineEditTextToSend;
        break;
    case 3:
        sendEdit = static_cast<Ui::CommunicationClass3*>(m_ui)->lineEditTextToSend;
        break;
    case 4:
        sendEdit = static_cast<Ui::CommunicationClass4*>(m_ui)->lineEditTextToSend;
        break;
    case 5:
        sendEdit = static_cast<Ui::CommunicationClass5*>(m_ui)->lineEditTextToSend;
        break;
    }

    if (sendEdit) {
        sendEdit->clear();
    }
}

void CommunicationWindow::handleClearTxRx()
{
    // 根据窗口ID获取显示控件
    QLabel* txDisplay = nullptr;
    QLabel* rxDisplay = nullptr;

    switch (m_windowId) {
    case 1:
        txDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->tx_display;
        rxDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->rx_display;
        break;
    case 2:
        txDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->tx_display;
        rxDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->rx_display;
        break;
    case 3:
        txDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->tx_display;
        rxDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->rx_display;
        break;
    case 4:
        txDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->tx_display;
        rxDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->rx_display;
        break;
    case 5:
        txDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->tx_display;
        rxDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->rx_display;
        break;
    }

    if (txDisplay) txDisplay->clear();
    if (rxDisplay) rxDisplay->clear();
    m_rx_num = 0;
    m_tx_num = 0;
}

void CommunicationWindow::handleRecvData(const QByteArray& data, bool hexDisplay)
{
    if (!data.isEmpty()) {
        QString result;
        if (hexDisplay) {
            for (quint8 byte : data) {
                result += QString("%1 ")
                    .arg(byte, 2, 16, QLatin1Char('0'))
                    .toUpper();
            }
        }
        else {
            result = QString(data);
        }
        m_rx_num += data.size();

        // 根据窗口ID获取显示控件
        QLabel* rxDisplay = nullptr;
        switch (m_windowId) {
        case 1:
            rxDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->rx_display;
            break;
        case 2:
            rxDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->rx_display;
            break;
        case 3:
            rxDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->rx_display;
            break;
        case 4:
            rxDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->rx_display;
            break;
        case 5:
            rxDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->rx_display;
            break;
        }

        if (rxDisplay) {
            rxDisplay->setText("rx:" + QString::number(m_rx_num));
            rxDisplay->setAlignment(Qt::AlignCenter);
        }

        appendLog(result, false);
    }
}

void CommunicationWindow::appendLog(const QString& str, bool isSend)
{
    // 根据窗口ID获取日志显示控件
    QTextEdit* logDisplay = nullptr;
    switch (m_windowId) {
    case 1:
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->log_display;
        break;
    case 2:
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->log_display;
        break;
    case 3:
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->log_display;
        break;
    case 4:
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->log_display;
        break;
    case 5:
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->log_display;
        break;
    }

    if (!logDisplay) return;

    QString append_str;
    append_str += QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");

    if (isSend == true) {
        append_str += "  SEND";
        logDisplay->append(append_str);
        logDisplay->setTextColor(Qt::blue);
        logDisplay->append(str);
        logDisplay->setTextColor(Qt::black);
    }
    else {
        append_str += "  RECV";
        logDisplay->append(append_str);
        logDisplay->setTextColor(Qt::darkCyan);
        logDisplay->append(str);
        logDisplay->setTextColor(Qt::black);
    }
}

void CommunicationWindow::handleSendData()
{
    qint32 send_byte_size = 0;
    QString displayStr;
    QByteArray write_buff;

    // 根据窗口ID获取发送数据控件
    QTextEdit* sendDisplay = nullptr;
    QLabel* txDisplay = nullptr;

    switch (m_windowId) {
    case 1:
        sendDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->send_data_display;
        txDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->tx_display;
        break;
    case 2:
        sendDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->send_data_display;
        txDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->tx_display;
        break;
    case 3:
        sendDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->send_data_display;
        txDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->tx_display;
        break;
    case 4:
        sendDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->send_data_display;
        txDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->tx_display;
        break;
    case 5:
        sendDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->send_data_display;
        txDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->tx_display;
        break;
    }

    if (!sendDisplay || !txDisplay) return;

    QString data_buff = sendDisplay->toPlainText();
    if (!m_serialPort->isConnected()) {
        QMessageBox::information(this, (QString::fromLocal8Bit("提示")), (QString::fromLocal8Bit("请先打开串口")));
    }
    else {
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
        m_tx_num += send_byte_size;
        txDisplay->setText("tx:" + QString::number(m_tx_num));
        txDisplay->setAlignment(Qt::AlignCenter);
        appendLog(displayStr, true);
    }
}

void CommunicationWindow::handleClearLog()
{
    // 根据窗口ID获取日志显示控件
    QTextEdit* logDisplay = nullptr;
    switch (m_windowId) {
    case 1:
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->log_display;
        break;
    case 2:
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->log_display;
        break;
    case 3:
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->log_display;
        break;
    case 4:
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->log_display;
        break;
    case 5:
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->log_display;
        break;
    }

    if (logDisplay) {
        logDisplay->clear();
    }
}

void CommunicationWindow::handleClearTxData()
{
    // 根据窗口ID获取发送数据控件
    QTextEdit* sendDisplay = nullptr;
    switch (m_windowId) {
    case 1:
        sendDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->send_data_display;
        break;
    case 2:
        sendDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->send_data_display;
        break;
    case 3:
        sendDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->send_data_display;
        break;
    case 4:
        sendDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->send_data_display;
        break;
    case 5:
        sendDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->send_data_display;
        break;
    }

    if (sendDisplay) {
        sendDisplay->clear();
    }
}

void CommunicationWindow::handleOpenSerial()
{
    // 根据窗口ID获取UI控件
    QPushButton* serialSwitch = nullptr;
    QPushButton* configButton = nullptr;

    switch (m_windowId) {
    case 1:
        serialSwitch = static_cast<Ui::CommunicationClass1*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass1*>(m_ui)->serialConfigButton;
        break;
    case 2:
        serialSwitch = static_cast<Ui::CommunicationClass2*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass2*>(m_ui)->serialConfigButton;
        break;
    case 3:
        serialSwitch = static_cast<Ui::CommunicationClass3*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass3*>(m_ui)->serialConfigButton;
        break;
    case 4:
        serialSwitch = static_cast<Ui::CommunicationClass4*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass4*>(m_ui)->serialConfigButton;
        break;
    case 5:
        serialSwitch = static_cast<Ui::CommunicationClass5*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass5*>(m_ui)->serialConfigButton;
        break;
    }

    if (!serialSwitch || !configButton) return;

    if (serialSwitch->text() == (QString::fromLocal8Bit("打开串口"))) {
        configButton->setEnabled(false);
        m_serialPort->portConnect(getSerialParaList());
    }
    else {
        m_serialPort->disconnect();
        configButton->setEnabled(true);
        serialSwitch->setText(QString::fromLocal8Bit("打开串口"));
    }
}

void CommunicationWindow::handleOpenSerialResult(bool result, const QString& errStr)
{
    // 根据窗口ID获取UI控件
    QPushButton* serialSwitch = nullptr;
    QPushButton* configButton = nullptr;

    switch (m_windowId) {
    case 1:
        serialSwitch = static_cast<Ui::CommunicationClass1*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass1*>(m_ui)->serialConfigButton;
        break;
    case 2:
        serialSwitch = static_cast<Ui::CommunicationClass2*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass2*>(m_ui)->serialConfigButton;
        break;
    case 3:
        serialSwitch = static_cast<Ui::CommunicationClass3*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass3*>(m_ui)->serialConfigButton;
        break;
    case 4:
        serialSwitch = static_cast<Ui::CommunicationClass4*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass4*>(m_ui)->serialConfigButton;
        break;
    case 5:
        serialSwitch = static_cast<Ui::CommunicationClass5*>(m_ui)->serial_switch;
        configButton = static_cast<Ui::CommunicationClass5*>(m_ui)->serialConfigButton;
        break;
    }

    if (!serialSwitch || !configButton) return;

    if (!result) {
        QMessageBox::warning(this, (QString::fromLocal8Bit("警告")), (QString::fromLocal8Bit("打开串口失败")));
        configButton->setEnabled(true);
    }
    else {
        serialSwitch->setText(QString::fromLocal8Bit("关闭串口"));
        configButton->setEnabled(false);
    }
}

void CommunicationWindow::initSerial()
{
    m_serialPort = new SerialPort(this);
    m_settings = new QSettings("config.ini", QSettings::IniFormat, this);
    m_tx_num = 0;
    m_rx_num = 0;

    // 清空显示区
    QTextEdit* sendDisplay = nullptr;
    QTextEdit* logDisplay = nullptr;

    switch (m_windowId) {
    case 1:
        sendDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->send_data_display;
        logDisplay = static_cast<Ui::CommunicationClass1*>(m_ui)->log_display;
        break;
    case 2:
        sendDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->send_data_display;
        logDisplay = static_cast<Ui::CommunicationClass2*>(m_ui)->log_display;
        break;
    case 3:
        sendDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->send_data_display;
        logDisplay = static_cast<Ui::CommunicationClass3*>(m_ui)->log_display;
        break;
    case 4:
        sendDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->send_data_display;
        logDisplay = static_cast<Ui::CommunicationClass4*>(m_ui)->log_display;
        break;
    case 5:
        sendDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->send_data_display;
        logDisplay = static_cast<Ui::CommunicationClass5*>(m_ui)->log_display;
        break;
    }

    if (sendDisplay) sendDisplay->clear();
    if (logDisplay) logDisplay->clear();
}

QVariantList CommunicationWindow::getSerialParaList()
{
    QVariantList paraList;
    paraList.append(m_serialPortName);
    paraList.append(m_serialBaudRate);
    paraList.append(static_cast<int>(m_serialDataBits));
    paraList.append(static_cast<int>(m_serialParity));
    paraList.append(static_cast<int>(m_serialStopBits));
    return paraList;
}

QVariantList CommunicationWindow::getTcpParaList()
{
    QVariantList paraList;
    paraList.append(m_tcpIp);
    paraList.append(m_tcpPort);
    return paraList;
}

void CommunicationWindow::handleSerialDataReceived(const QByteArray& data)
{
    handleRecvData(data, m_serialHexDisplay);
}

void CommunicationWindow::handleTcpDataReceived(const QByteArray& data)
{
    onDataReceived(data, m_tcpHexDisplay);
}