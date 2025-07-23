#include "Tcp.h"

Tcp::Tcp(QObject* parent) : ICommunication(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ICommunication::readData);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        emit connectStatus(true, "no error");
        });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        emit connectStatus(false, "disconnected");
        });

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        [this](QAbstractSocket::SocketError) {
            emit connectStatus(false, socket->errorString());
        });
}

Tcp::~Tcp()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

void Tcp::portConnect(const QVariantList& params)
{
    socket->connectToHost(params.at(0).toString(), params.at(1).toInt());
}

void Tcp::doDisconnect()
{
    socket->disconnectFromHost();
}

bool Tcp::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

qint64 Tcp::writeData(const QByteArray& data)
{
    return socket->write(data);
}

QIODevice* Tcp::getDevice()
{
    return socket;
}