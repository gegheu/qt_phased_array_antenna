#pragma once
#include <QObject>
#include <QMap>
#include "IProtocol.h"
#include "VFProtocol.h"
#include "AntennaProtocol.h"

class CommunicationManager : public QObject {
    Q_OBJECT
public:
    explicit CommunicationManager(QObject* parent = nullptr);

    VFProtocol* getVFProtocol() const { return m_vfProtocol; }
    AntennaProtocol* getAntennaProtocol() const { return m_antennaProtocol; }

public slots:
    void onDataReceived(const QByteArray& data);

private:
    QMap<quint8, IProtocol*> m_protocols;
    VFProtocol* m_vfProtocol;
    AntennaProtocol* m_antennaProtocol;

signals:
    void VFRecieve(const VFProtocol::VarFreqResponse& response);
    void AnteRecieve(const AntennaProtocol::AnteModuleFrame& packet);
};