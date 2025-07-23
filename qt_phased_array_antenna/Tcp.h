#pragma once
#include "icommunication.h"
#include <QTcpSocket>

class Tcp : public ICommunication
{
    Q_OBJECT
public:
    Tcp(QObject* parent = nullptr);
    ~Tcp() override;

    void portConnect(const QVariantList& params) override;

    bool isConnected() const override;

protected:
    qint64 writeData(const QByteArray& data) override;
    QIODevice* getDevice() override;
    void doDisconnect() override;

private:
    QTcpSocket* socket = nullptr;
};