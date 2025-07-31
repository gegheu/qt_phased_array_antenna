#include"CommManager.h"

CommunicationManager::CommunicationManager(QObject* parent) : QObject(parent) 
{
	m_protocols.insert(VF_FRAMEHEAD, new VFProtocol(this));
	m_protocols.insert(ANTE_FRAMEHEAD1, new AntennaProtocol(this));

	/*新增其他协议*/
	connect(qobject_cast<VFProtocol*>(m_protocols[VF_FRAMEHEAD]), &VFProtocol::VFEvent,this, [this](const VFProtocol::VFModuleFrame& data) {
			emit VFRecieve(data); // 直接转发
		});
	connect(qobject_cast<AntennaProtocol*>(m_protocols[ANTE_FRAMEHEAD1]), &AntennaProtocol::AnteEvent,this, [this](const AntennaProtocol::AnteModuleFrame& data) {
			emit AnteRecieve(data); // 直接转发
		});
}


void CommunicationManager::onDataReceived(const QByteArray& data)
{
	quint8 header = static_cast<quint8>(data[0]);
	if (m_protocols.contains(header)) {
		IProtocol* protocol = m_protocols[header];
		protocol->parseResponse(data);
	}
	else {
		/*异常处理*/
	}
}

void CommunicationManager::test()
{
	int a = 0;
}