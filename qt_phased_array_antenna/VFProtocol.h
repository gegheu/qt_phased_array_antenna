#pragma once
#include "iProtocol.h"

#define VAR_FREQ_FRAME_HEAD 0x5A
#define VAR_FREQ_FRAME_TAIL 0xA5

// 频率范围
#define RX_FREQ_MIN 17700
#define RX_FREQ_MAX 21200
#define TX_FREQ_MIN 28225
#define TX_FREQ_MAX 30275

class VFProtocol : public IProtocol {
    Q_OBJECT
public:
    explicit VFProtocol(QObject* parent = nullptr);

    QByteArray buildCommand(const QByteArray& data, const QVariantMap& para = {}) override;
    void parseResponse(const QByteArray& data) override;

    // 变频率协议结构体
    struct VarFreqFrame {
        quint8 frameHead = VAR_FREQ_FRAME_HEAD;
        quint8 cmd;
        quint16 data;
        quint8 frameTail = VAR_FREQ_FRAME_TAIL;
    };

    // 响应结构体
    struct VarFreqResponse {
        quint8 frameHead;
        quint8 cmd;
        quint8 frameTail;
        bool isValid;
    };

    // 命令号枚举
    enum VarFreqCmd {
        RxFreq = 0x01,
        TxFreq = 0x02
    };

    // 辅助函数
    static inline bool isValidRxFrequency(quint32 freqMHz) {
        return (freqMHz >= RX_FREQ_MIN && freqMHz <= RX_FREQ_MAX);
    }

    static inline bool isValidTxFrequency(quint32 freqMHz) {
        return (freqMHz >= TX_FREQ_MIN && freqMHz <= TX_FREQ_MAX);
    }

    static inline quint32 clampRxFrequency(quint32 freqMHz) {
        if (freqMHz < RX_FREQ_MIN) return RX_FREQ_MIN;
        if (freqMHz > RX_FREQ_MAX) return RX_FREQ_MAX;
        return freqMHz;
    }

    static inline quint32 clampTxFrequency(quint32 freqMHz) {
        if (freqMHz < TX_FREQ_MIN) return TX_FREQ_MIN;
        if (freqMHz > TX_FREQ_MAX) return TX_FREQ_MAX;
        return freqMHz;
    }

signals:
    void varFreqResponse(const VarFreqResponse& response);
};