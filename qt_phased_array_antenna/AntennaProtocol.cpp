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
	frameStr.frameHead1 = static_cast<quint8>(data[offset]);					offset += sizeof(frameStr.frameHead1);
	frameStr.frameHead2 = static_cast<quint8>(data[offset]);					offset += sizeof(frameStr.frameHead2);
	frameStr.cmd = static_cast<quint8>(data[offset]);							offset += sizeof(frameStr.cmd);
	frameStr.frameLength = static_cast<quint8>(data[offset]);					offset += sizeof(frameStr.frameLength);
	frameStr.data = data.mid(offset, frameStr.frameLength - DATA_INFO_LENGTH);	offset += frameStr.frameLength - DATA_INFO_LENGTH;
	frameStr.check= static_cast<quint8>(data[offset]);							offset += sizeof(frameStr.check);
	frameStr.frameTail = static_cast<quint8>(data[offset]);
	if (frameStr.check != frameCheck(data.mid(2, frameStr.frameLength), CheckType::Sum)) {
		//–£—È“Ï≥£
	}
	emit AnteEvent(frameStr);
}