#include "Tcp.h"

Tcp::Tcp(QObject* parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &Tcp::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &Tcp::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Tcp::onDisconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        this, &Tcp::onErrorOccurred);
}

Tcp::~Tcp()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
    delete socket;
}

void Tcp::connectToHost(const QString& host, int port)
{
    socket->connectToHost(host, port);
}

void Tcp::disconnect()
{
    socket->disconnectFromHost();
}

bool Tcp::isConnected()
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

void Tcp::write(const QByteArray& data)
{
    if (isConnected()) {
        socket->write(data);
    }
}

void Tcp::onReadyRead()
{
    QByteArray data = socket->readAll();
    emit dataReceived(data);
}

void Tcp::onConnected()
{
    emit connected();
}

void Tcp::onDisconnected()
{
    emit disconnected();
}


void Tcp::onErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    emit connectError(socket->errorString());
}