#pragma once
#include <QSerialPort>
#include "iCommunication.h"

class SerialPort : public ICommunication
{
    Q_OBJECT
public:
    explicit SerialPort(const QString& instanceId = "", QObject* parent = nullptr);
    ~SerialPort() override;

    void portConnect(const QVariantList& params) override;
    bool isConnected() const override;

protected:
    qint64 writeData(const QByteArray& data) override;
    QIODevice* getDevice() override;
    void doDisconnect() override;

private:
    QSerialPort* serial = nullptr;
};