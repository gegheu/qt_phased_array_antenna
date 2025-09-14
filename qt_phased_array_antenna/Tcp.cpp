#include "Tcp.h"

Tcp::Tcp(const QString& instanceId, QObject* parent)
    : ICommunication(instanceId, parent),
    socket(new QTcpSocket(this)),
    m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &Tcp::handleTimeout);
    connect(socket, &QTcpSocket::readyRead, this, &ICommunication::readData);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        m_timeoutTimer->stop();
        emit connectStatus(this->instanceId(), true, "no error");
        });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        m_timeoutTimer->stop();
        emit connectStatus(this->instanceId(), false, QStringLiteral("连接已断开"));
        });

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        [this](QAbstractSocket::SocketError error) {
            m_timeoutTimer->stop();
            QString errorStr;
            switch (error) {
            case QAbstractSocket::ConnectionRefusedError:
                errorStr = QStringLiteral("连接被拒绝");
                break;
            case QAbstractSocket::RemoteHostClosedError:
                errorStr = QStringLiteral("远程主机关闭连接");
                break;
            case QAbstractSocket::HostNotFoundError:
                errorStr = QStringLiteral("主机未找到");
                break;
            case QAbstractSocket::SocketAccessError:
                errorStr = QStringLiteral("套接字访问错误");
                break;
            case QAbstractSocket::SocketResourceError:
                errorStr = QStringLiteral("套接字资源错误");
                break;
            case QAbstractSocket::SocketTimeoutError:
                errorStr = QStringLiteral("连接超时");
                break;
            case QAbstractSocket::NetworkError:
                errorStr = QStringLiteral("网络错误");
                break;
            default:
                errorStr = QStringLiteral("未知错误: %1").arg(error);
                break;
            }
            emit connectStatus(this->instanceId(), false, errorStr);
        });
}

Tcp::~Tcp()
{
    if (isConnected()) {
        doDisconnect();
    }
}

void Tcp::portConnect(const QVariantList& params)
{
    // 确保没有正在进行的连接
    if (isConnected() || isConnecting()) {
        return;
    }

    socket->connectToHost(params.at(0).toString(), params.at(1).toInt());
    m_timeoutTimer->start(5000);
}

void Tcp::doDisconnect()
{
    m_timeoutTimer->stop();
    if (isConnecting()) {
        socket->abort();

        emit connectStatus(this->instanceId(), false, QStringLiteral("手动断开"));
    }
    else {
        socket->disconnectFromHost();
        // 这里不立即发送状态信号，等待 disconnected 信号
    }
}

bool Tcp::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

bool Tcp::isConnecting() const
{
    return socket->state() == QAbstractSocket::ConnectingState;
}

qint64 Tcp::writeData(const QByteArray& data)
{
    return socket->write(data);
}

QIODevice* Tcp::getDevice()
{
    return socket;
}

void Tcp::handleTimeout()
{
    if (isConnecting()) {
        socket->abort(); // 中止连接
        emit connectStatus(this->instanceId(), false, QStringLiteral("连接超时"));
    }
}

void Tcp::abortConnection()
{
    m_timeoutTimer->stop();
    if (isConnecting()) {
        socket->abort();
        emit connectStatus(instanceId(), false, QStringLiteral("连接已取消"));
    }
}