#include"VFProtocol.h"

VFProtocol::VFProtocol(QObject* parent) : IProtocol(parent) {}

QByteArray VFProtocol::buildCommand(const QByteArray& data, const QVariantMap& para)
{
	VFModuleFrame frameStr;
	
	frameStr.cmd = para[VF_CMD].toUInt();
	frameStr.frameLength = data.size();
	frameStr.data = data;

	return revertFrame(frameStr);
}

QByteArray VFProtocol::revertFrame(const VFModuleFrame& data)
{
	QByteArray packet;
	packet.append(data.frameHead);
	packet.append(data.cmd);
	packet.append(data.frameLength);
	packet.append(data.data);

	return packet;
}

void VFProtocol::parseResponse(const QByteArray& data)
{
	quint8 offset = 0;
	VFModuleFrame frameStr;
	frameStr.frameHead = static_cast<quint8>(data[offset]);		offset += sizeof(frameStr.frameHead);
	frameStr.cmd = static_cast<quint8>(data[offset]);			offset += sizeof(frameStr.cmd);
	frameStr.frameLength = static_cast<quint8>(data[offset]);	offset += sizeof(frameStr.frameLength);
	frameStr.data = data.mid(offset, frameStr.frameLength);
	emit VFEvent(frameStr);
}