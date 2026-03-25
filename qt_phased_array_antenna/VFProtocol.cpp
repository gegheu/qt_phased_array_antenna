#include "VFProtocol.h"

VFProtocol::VFProtocol(QObject* parent) : IProtocol(parent) {}

QByteArray VFProtocol::buildCommand(const QByteArray& data, const QVariantMap& para)
{
    Q_UNUSED(para);

    QByteArray packet;
    packet.append(static_cast<quint8>(VAR_FREQ_FRAME_HEAD));
    packet.append(data[0]);  // 命令号
    packet.append(data[1]);  // 数据高字节
    packet.append(data[2]);  // 数据低字节
    packet.append(static_cast<quint8>(VAR_FREQ_FRAME_TAIL));

    return packet;
}

void VFProtocol::parseResponse(const QByteArray& data)
{
    VarFreqResponse response;
    response.isValid = false;

    if (data.size() >= 3 &&
        static_cast<quint8>(data[0]) == VAR_FREQ_FRAME_HEAD &&
        static_cast<quint8>(data[2]) == VAR_FREQ_FRAME_TAIL) {

        response.frameHead = static_cast<quint8>(data[0]);
        response.cmd = static_cast<quint8>(data[1]);
        response.frameTail = static_cast<quint8>(data[2]);
        response.isValid = true;

        emit varFreqResponse(response);
    }
}