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
	frameStr.frameHead = data.mid(offset, sizeof(frameStr.frameHead)).toUInt();		offset += frameStr.frameHead;
	frameStr.cmd = data.mid(offset, sizeof(frameStr.cmd)).toUInt();					offset += frameStr.cmd;
	frameStr.frameLength = data.mid(offset, sizeof(frameStr.frameLength)).toUInt();	offset += frameStr.frameLength;
	frameStr.data = data.mid(offset, frameStr.frameLength);
}