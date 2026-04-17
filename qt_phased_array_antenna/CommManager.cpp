#include "CommManager.h"

CommunicationManager& CommunicationManager::instance() {
    static CommunicationManager _instance;
    return _instance;
}

CommunicationManager::CommunicationManager(QObject* parent) : QObject(parent) {
    // 1. 初始化固有的协议实例
    m_antennaProtocol = new AntennaProtocol(this);
    m_powerProtocol = new PowerProtocol(this);
    m_vfProtocol = new VFProtocol(this);

    // 2. 注册协议映射关系
    addProtocol(0xEB, m_antennaProtocol);
    addProtocol(0x1A, m_powerProtocol);
    addProtocol(0x5A, m_vfProtocol);
}

void CommunicationManager::addDevice(const QString& id, ICommunication* device) {
    if (!device) return;
    m_devices.insert(id, device);

    // 【关键连接】：将任何一个串口的 dataReceived 都导向管理器的统一处理槽
    connect(device, &ICommunication::dataReceived, this, &CommunicationManager::onDataReceived);
}

void CommunicationManager::addProtocol(quint8 header, IProtocol* protocol) {
    if (protocol) m_protocolMap.insert(header, protocol);
}

ICommunication* CommunicationManager::getDevice(const QString& id) const {
    return m_devices.value(id, nullptr);
}

void CommunicationManager::onDataReceived(const QString& instanceId, const QByteArray& data) {
    if (data.isEmpty()) return;

    // 根据数据帧头分发
    quint8 header = static_cast<quint8>(data[0]);
    if (m_protocolMap.contains(header)) {
        // 调用具体的 parseResponse
        m_protocolMap[header]->parseResponse(data);
    }
}