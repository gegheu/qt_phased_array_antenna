#pragma once
#include <QObject>
#include <QMap>
#include"VFProtocol.h"
#include"AntennaProtocol.h"


class CommunicationManager : public QObject {
    Q_OBJECT
public:
    CommunicationManager(QObject* parent = nullptr);

public slots:
    void onDataReceived(const QByteArray& data);
    void test();

private:
    QMap<qint64, IProtocol*> m_protocols; 

signals:
    void VFRecieve(const VFProtocol::VFModuleFrame& packet);
    void AnteRecieve(const AntennaProtocol::AnteModuleFrame& packet);
};