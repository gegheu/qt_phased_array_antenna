#pragma once

#include "iProtocol.h"

// 电源协议常量定义
#define POWER_FRAME_HEAD1 0x1A
#define POWER_FRAME_HEAD2 0xCF
#define POWER_ID_HIGH 0x55
#define POWER_ID_LOW 0xAA
#define POWER_FRAME_TAIL1 0xFC
#define POWER_FRAME_TAIL2 0x1D

// 业务码
#define POWER_CMD_DOWN 0xEC      // 下行控制包
#define POWER_CMD_UP 0xBD        // 上行状态包

class PowerProtocol : public IProtocol
{
    Q_OBJECT

public:
    explicit PowerProtocol(QObject* parent = nullptr);

    // IProtocol 接口实现
    QByteArray buildCommand(const QByteArray& data, const QVariantMap& para = {}) override;
    void parseResponse(const QByteArray& data) override;

    // 电源状态结构体
    struct PowerStatusFrame {
        quint8 frameHead1 = POWER_FRAME_HEAD1;
        quint8 frameHead2 = POWER_FRAME_HEAD2;
        quint8 idHigh = POWER_ID_HIGH;
        quint8 idLow = POWER_ID_LOW;
        quint8 cmd = POWER_CMD_UP;

        // 6个通道的电压和电流 (每个通道4字节)
        struct ChannelData {
            quint16 voltage;     // 电压值 (0.01V)
            quint16 current;     // 电流值 (0.01A)
        } channels[6];

        quint16 inputPower;      // 输入功率 (1W)
        quint16 temperature;     // 温度 (0.1°C)
        quint8 reserved = 0x00;
        quint8 frameTail1 = POWER_FRAME_TAIL1;
        quint8 frameTail2 = POWER_FRAME_TAIL2;
    };

    // 通道控制值定义
    enum ChannelControlValue {
        CH1_CLOSE = 0x15, CH1_OPEN = 0x1A,   // 12V1
        CH2_CLOSE = 0x25, CH2_OPEN = 0x2A,   // 12V2
        CH3_CLOSE = 0x35, CH3_OPEN = 0x3A,   // 12V3
        CH4_CLOSE = 0x45, CH4_OPEN = 0x4A,   // 12V4
        CH5_CLOSE = 0x55, CH5_OPEN = 0x5A,   // 24V1
        CH6_CLOSE = 0x65, CH6_OPEN = 0x6A    // 24V2
    };

    // 通道编号
    enum PowerChannel {
        CHANNEL_12V1 = 0,
        CHANNEL_12V2 = 1,
        CHANNEL_12V3 = 2,
        CHANNEL_12V4 = 3,
        CHANNEL_24V1 = 4,
        CHANNEL_24V2 = 5
    };

    // 辅助函数
    static inline quint8 getChannelOpenValue(int channel) {
        static const quint8 openValues[] = { 0x1A, 0x2A, 0x3A, 0x4A, 0x5A, 0x6A };
        return (channel >= 0 && channel < 6) ? openValues[channel] : 0x00;
    }

    static inline quint8 getChannelCloseValue(int channel) {
        static const quint8 closeValues[] = { 0x15, 0x25, 0x35, 0x45, 0x55, 0x65 };
        return (channel >= 0 && channel < 6) ? closeValues[channel] : 0x00;
    }

    static inline double voltageToDouble(quint16 raw) { return raw / 100.0; }
    static inline double currentToDouble(quint16 raw) { return raw / 100.0; }
    static inline double powerToDouble(quint16 raw) { return static_cast<double>(raw); }
    static inline double temperatureToDouble(quint16 raw) { return raw / 10.0; }

signals:
    void powerStatusReceived(const PowerStatusFrame& status);
};