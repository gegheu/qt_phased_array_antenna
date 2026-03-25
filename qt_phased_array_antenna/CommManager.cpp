#include "CommManager.h"

CommunicationManager::CommunicationManager(QObject* parent)
    : QObject(parent)
    , m_vfProtocol(nullptr)
    , m_antennaProtocol(nullptr)
{
    // 创建协议实例
    m_vfProtocol = new VFProtocol(this);
    m_antennaProtocol = new AntennaProtocol(this);

    // 注册协议映射（帧头 → 协议）
    m_protocols.insert(0x5A, m_vfProtocol);
    m_protocols.insert(0xEB, m_antennaProtocol);

    // 连接协议信号并转发（转发完整数据）
    connect(m_vfProtocol, &VFProtocol::varFreqResponse,
        this, [this](const VFProtocol::VarFreqResponse& response) {
            emit VFRecieve(response);
        });

    connect(m_antennaProtocol, &AntennaProtocol::AnteEvent,
        this, [this](const AntennaProtocol::AnteModuleFrame& frame) {
            emit AnteRecieve(frame);
        });
}

void CommunicationManager::onDataReceived(const QByteArray& data)
{
    if (data.isEmpty()) return;

    quint8 header = static_cast<quint8>(data[0]);
    if (m_protocols.contains(header)) {
        m_protocols[header]->parseResponse(data);
    }
    else {
        // 异常处理
    }
 
}