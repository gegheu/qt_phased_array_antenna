#pragma once
#include "iCommunication.h"
#include <QTcpSocket>
#include <QTimer> 

class Tcp : public ICommunication
{
    Q_OBJECT
public:
    Tcp(QObject* parent = nullptr);
    ~Tcp() override;
    void portConnect(const QVariantList& params) override;
    bool isConnected() const override;
    bool isConnecting() const;
    void abortConnection();

private slots:
    void handleTimeout();




protected:
    qint64 writeData(const QByteArray& data) override;
    QIODevice* getDevice() override;
    void doDisconnect() override;

private:
    QTcpSocket* socket = nullptr;
    QTimer* m_timeoutTimer;


};