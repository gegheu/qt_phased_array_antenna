#include"CommManager.h"
#include"VFProtocol.h"
#include"AntennaProtocol.h"

CommunicationManager::CommunicationManager(QObject* parent) : QObject(parent) 
{
	m_protocols.insert(VF_FRAMEHEAD, new VFProtocol(this));
	m_protocols.insert(ANTE_FRAMEHEAD1, new AntennaProtocol(this));

	/*新增其他协议*/
}


void CommunicationManager::onDataReceived(const QByteArray& data)
{
	quint8 header = data.left(1).toUInt();
	if (m_protocols.contains(header)) {
		IProtocol* protocol = m_protocols[header];
		protocol->parseResponse(data);
	}
	else {
		/*异常处理*/
	}
}