#pragma once
#include <QObject>
#include <QVariantMap>
#include <QIODevice>

class ICommunication : public QObject {
    Q_OBJECT
public:
    ICommunication(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ICommunication() = default;

    virtual void portConnect(const QVariantList& params) = 0;
    virtual bool isConnected() const = 0;

    qint64 write(const QByteArray& data) {
        if (isConnected()) {
            return writeData(data);
        }
        return -1;
    }

    void disconnect() {
        if (isConnected()) {
            doDisconnect();
        }
    }

    void readData() {
        QIODevice* device = getDevice();
        if (device) {
            QByteArray data = device->readAll();
            while (device->waitForReadyRead(10)) {
                data += device->readAll();
            }
            emit dataReceived(data);
        }
    }

signals:
    void connectStatus(bool flag, const QString& error);
    void dataReceived(const QByteArray& data);

protected:
    virtual qint64 writeData(const QByteArray& data) = 0;
    virtual QIODevice* getDevice() = 0;
    virtual void doDisconnect() = 0;
};