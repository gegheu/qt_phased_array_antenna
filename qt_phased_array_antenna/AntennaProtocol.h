#pragma once
#include"iProtocol.h"

#define ANTE_FRAMEHEAD1 0xEB
#define ANTE_FRAMEHEAD2 0x90
#define ANTE_FRAMETAIL 0x146F
#define ANTE_CMD "ANTE_CMD"

class AntennaProtocol : public IProtocol {
    Q_OBJECT
public:
    AntennaProtocol(QObject* parent = nullptr);
    QByteArray buildCommand(const QByteArray& data, const QVariantMap& para) override;
    void parseResponse(const QByteArray& data) override;

private:
    enum AnteCmdType {
        SwitchCtrl = 0x01,
        TRATTCtrl = 0x02,
        CVWrite = 0x03,
        CVRead = 0x04,
        TRATTCmdWrite = 0x05,
        TRATTCmdRead = 0x06,
        APWCmdWrite = 0x07,
        APWCmdRead = 0x08,
        BSCtrl = 0x09,
        ChipTempRead = 0x0C
    };

    enum AnteCmdResType {
        SwitchCtrlRes = 0x01,
        TRATTCtrlRes = 0x02,
        CVWriteRes = 0x03,
        CVReadRes = 0x04,
        TRATTCmdWriteRes = 0x05,
        TRATTCmdReadRes = 0x06,
        APWCmdWriteRes = 0x07,
        APWCmdReadRes = 0x08,
        BSCtrlRes = 0x09,
        TempRes = 0x0A,
        PDRes = 0x0B,
        ChipTempReadRes = 0x0C,
        SubTempCondRes = 0x0D,
        SubErrorRes = 0x0E,
        SelfChkRes = 0x31,
        SysErrorRes = 0x32
    };


    struct AnteModuleFrame {
        quint8 frameHead1 = ANTE_FRAMEHEAD1;
        quint8 frameHead2 = ANTE_FRAMEHEAD2;
        quint8 cmd;
        quint8 frameLength;
        QByteArray data;
        qint8 check;
        quint16 frameTail = ANTE_FRAMETAIL;
    };

    QByteArray revertFrame(const AnteModuleFrame& packet);

};