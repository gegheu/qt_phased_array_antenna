#pragma once
#include <QObject>
#include <QMap>
#include "iProtocol.h"
#include "icommunication.h"
#include "AntennaProtocol.h"
#include "PowerProtocol.h"
#include "VFProtocol.h"

class CommunicationManager : public QObject {
    Q_OBJECT
public:
    // 单例模式接口
    static CommunicationManager& instance();

    // 资源注册接口
    void addDevice(const QString& id, ICommunication* device);
    void addProtocol(quint8 header, IProtocol* protocol);

    // 获取资源接口（供页签 UI 使用）
    ICommunication* getDevice(const QString& id) const;
    AntennaProtocol* getAntennaProtocol() const { return m_antennaProtocol; }
    PowerProtocol* getPowerProtocol() const { return m_powerProtocol; }
    VFProtocol* getVFProtocol() const { return m_vfProtocol; }

public slots:
    // 核心分发槽：所有串口的数据都会汇聚到这里
    void onDataReceived(const QString& instanceId, const QByteArray& data);

private:
    explicit CommunicationManager(QObject* parent = nullptr);

    QMap<QString, ICommunication*> m_devices;    // 设备池
    QMap<quint8, IProtocol*> m_protocolMap;     // 协议表

    // 协议实例
    AntennaProtocol* m_antennaProtocol;
    PowerProtocol* m_powerProtocol;
    VFProtocol* m_vfProtocol;

signals:
    // 转发给 UI 的业务信号
    void antennaDataArrived(const AntennaProtocol::AnteModuleFrame& frame);
};
