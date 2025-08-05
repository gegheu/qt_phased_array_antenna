#include "Tcp.h"

Tcp::Tcp(QObject* parent) : ICommunication(parent)
{
    socket = new QTcpSocket(this);
    m_timeoutTimer = new QTimer(this); // 创建超时定时器
    m_timeoutTimer->setSingleShot(true); // 单次触发
    connect(m_timeoutTimer, &QTimer::timeout, this, &Tcp::handleTimeout);
    connect(socket, &QTcpSocket::readyRead, this, &ICommunication::readData);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        m_timeoutTimer->stop();
        emit connectStatus(true, "no error");
        });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        m_timeoutTimer->stop();
        emit connectStatus(false, "disconnected");
        });

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        [this](QAbstractSocket::SocketError) {
            m_timeoutTimer->stop();
            emit connectStatus(false, socket->errorString());
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
    }
    else {
        socket->disconnectFromHost();
    }
    emit connectStatus(false, "手动断开");
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
        emit connectStatus(false, "Connection timed out");
    }
}