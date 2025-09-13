#pragma once
#include <QObject>
#include <QVariantMap>
#include <QIODevice>
#include <QUuid>

class ICommunication : public QObject {
    Q_OBJECT
public:
    explicit ICommunication(const QString& instanceId = "", QObject* parent = nullptr);
    virtual ~ICommunication() = default;

    virtual void portConnect(const QVariantList& params) = 0;
    virtual bool isConnected() const = 0;

    QString instanceId() const { return m_instanceId; }
    void setInstanceId(const QString& id) { m_instanceId = id; }

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
        if (!device) return;

        QByteArray data = device->readAll();
        emit dataReceived(m_instanceId, data);
    }

signals:
    void connectStatus(const QString& instanceId, bool flag, const QString& error);
    void dataReceived(const QString& instanceId, const QByteArray& data);

protected:
    virtual qint64 writeData(const QByteArray& data) = 0;
    virtual QIODevice* getDevice() = 0;
    virtual void doDisconnect() = 0;

private:
    QString m_instanceId;
};