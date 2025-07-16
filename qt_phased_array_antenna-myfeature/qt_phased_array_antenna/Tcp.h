#pragma once
#include <QObject>
#include <QTcpSocket>

class Tcp : public QObject
{
    Q_OBJECT
public:
    Tcp(QObject* parent = nullptr);
    ~Tcp();

    void connectToHost(const QString& host, int port);
    void disconnect();
    bool isConnected();
    void write(const QByteArray& data);

signals:
    void dataReceived(const QByteArray& data);
    void connected();
    void disconnected();
    void connectError(const QString& errorInfo);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QTcpSocket* socket;
};