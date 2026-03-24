#include "PowerProtocol.h"

PowerProtocol::PowerProtocol(QObject* parent)
    : IProtocol(parent)
{
}

QByteArray PowerProtocol::buildCommand(const QByteArray& data, const QVariantMap& para)
{
    Q_UNUSED(para);

    QByteArray packet;

    // 帧头 (2字节)
    packet.append(static_cast<quint8>(POWER_FRAME_HEAD1));
    packet.append(static_cast<quint8>(POWER_FRAME_HEAD2));

    // 标识位 (2字节)
    packet.append(static_cast<quint8>(POWER_ID_HIGH));
    packet.append(static_cast<quint8>(POWER_ID_LOW));

    // 业务码: 0xEC 下行控制包
    packet.append(static_cast<quint8>(POWER_CMD_DOWN));

    // 数据段 (6字节控制数据)
    QByteArray controlData = data.left(6);
    controlData.append(6 - controlData.size(), 0x00);
    packet.append(controlData);

    // 帧尾 (2字节)
    packet.append(static_cast<quint8>(POWER_FRAME_TAIL1));
    packet.append(static_cast<quint8>(POWER_FRAME_TAIL2));

    return packet;
}

void PowerProtocol::parseResponse(const QByteArray& data)
{
    // 检查最小长度 (36字节)
    if (data.size() < 36) {
        return;
    }

    // 验证帧头
    if (static_cast<quint8>(data[0]) != POWER_FRAME_HEAD1 ||
        static_cast<quint8>(data[1]) != POWER_FRAME_HEAD2) {
        return;
    }

    // 验证标识位
    if (static_cast<quint8>(data[2]) != POWER_ID_HIGH ||
        static_cast<quint8>(data[3]) != POWER_ID_LOW) {
        return;
    }

    // 验证业务码 (只处理上行状态包)
    if (static_cast<quint8>(data[4]) != POWER_CMD_UP) {
        return;
    }

    PowerStatusFrame frame;

    // 解析帧头
    frame.frameHead1 = static_cast<quint8>(data[0]);
    frame.frameHead2 = static_cast<quint8>(data[1]);

    // 解析标识位
    frame.idHigh = static_cast<quint8>(data[2]);
    frame.idLow = static_cast<quint8>(data[3]);

    // 解析业务码
    frame.cmd = static_cast<quint8>(data[4]);

    // 解析6个通道的电压和电流 (索引5-28)
    int offset = 5;
    for (int i = 0; i < 6; i++) {
        quint16 voltageRaw = (static_cast<quint8>(data[offset]) << 8) |
            static_cast<quint8>(data[offset + 1]);
        quint16 currentRaw = (static_cast<quint8>(data[offset + 2]) << 8) |
            static_cast<quint8>(data[offset + 3]);

        frame.channels[i].voltage = voltageRaw;
        frame.channels[i].current = currentRaw;

        offset += 4;
    }

    // 解析输入功率 (索引30-31)
    frame.inputPower = (static_cast<quint8>(data[30]) << 8) |
        static_cast<quint8>(data[31]);

    // 解析温度 (索引32-33)
    frame.temperature = (static_cast<quint8>(data[32]) << 8) |
        static_cast<quint8>(data[33]);

    // 解析备用字节 (索引34)
    frame.reserved = static_cast<quint8>(data[34]);

    // 解析帧尾 (索引35-36)
    frame.frameTail1 = static_cast<quint8>(data[35]);
    frame.frameTail2 = static_cast<quint8>(data[36]);

    // 验证帧尾
    if (frame.frameTail1 != POWER_FRAME_TAIL1 ||
        frame.frameTail2 != POWER_FRAME_TAIL2) {
        return;
    }

    // 发送解析结果
    emit powerStatusReceived(frame);
}