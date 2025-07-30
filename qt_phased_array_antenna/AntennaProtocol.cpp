#include"AntennaProtocol.h"

AntennaProtocol::AntennaProtocol(QObject* parent) : IProtocol(parent) {}

QByteArray AntennaProtocol::buildCommand(const QByteArray& data, const QVariantMap& para)
{
	AnteModuleFrame frameStr;

	frameStr.cmd = para[ANTE_CMD].toUInt();
	frameStr.frameLength = data.size();
	frameStr.data = data;

	return revertFrame(frameStr);
}

QByteArray AntennaProtocol::revertFrame(const AnteModuleFrame& data)
{
	QByteArray packet;
	packet.append(data.frameHead1);
	packet.append(data.frameHead2);
	packet.append(data.cmd);
	packet.append(data.frameLength);
	packet.append(data.data);

	return packet;
}
#define DATA_INFO_LENGTH 4
void AntennaProtocol::parseResponse(const QByteArray& data)
{
	quint8 offset = 0;
	AnteModuleFrame frameStr;
	frameStr.frameHead1 = data.mid(offset, sizeof(frameStr.frameHead1)).toUInt();		offset += sizeof(frameStr.frameHead1);
	frameStr.frameHead2 = data.mid(offset, sizeof(frameStr.frameHead2)).toUInt();		offset += sizeof(frameStr.frameHead2);
	frameStr.cmd = data.mid(offset, sizeof(frameStr.cmd)).toUInt();						offset += sizeof(frameStr.cmd);
	frameStr.frameLength = data.mid(offset, sizeof(frameStr.frameLength)).toUInt();		offset += sizeof(frameStr.frameLength);
	frameStr.data = data.mid(offset, frameStr.frameLength - DATA_INFO_LENGTH);			offset += frameStr.frameLength - DATA_INFO_LENGTH;
	frameStr.check= data.mid(offset, sizeof(frameStr.check)).toUInt();					offset += sizeof(frameStr.check);
	frameStr.frameTail = data.mid(offset, sizeof(frameStr.frameTail)).toUInt();			
	if (frameStr.check != frameCheck(data.mid(2, frameStr.frameLength), Sum)) {
		//–£—È“Ï≥£
	}
}