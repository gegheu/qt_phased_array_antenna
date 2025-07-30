#pragma once
#include"iProtocol.h"

#define VF_FRAMEHEAD 0x55
#define VF_CMD "VF_CMD"

class VFProtocol : public IProtocol {
    Q_OBJECT
public:
    VFProtocol(QObject* parent = nullptr);
    QByteArray buildCommand(const QByteArray& data, const QVariantMap& para) override;
    void parseResponse(const QByteArray& data) override;

private:
    enum VFCmdType {
        SwitchCtrl = 0x01,
        LOCtrl12 = 0x02,
        LOCtrl11 = 0x03,
        LOCtrl22 = 0x04,
        LOCtrl21 = 0x05,
        LOCtrl32 = 0x06,
        LOCtrl31 = 0x07,
        LOCtrl42 = 0x08,
        LOCtrl41 = 0x09,
        ATTCtrl1 = 0x0A,
        ATTCtrl2 = 0x0B,
        ATTCtrl3 = 0x0C,
        ATTCtrl4 = 0x0D,
    };
    enum VFCmdTypeRes {
        SwitchCtrlRes = 0x81,
        LOCtrl12Res = 0x82,
        LOCtrl11Res = 0x83,
        LOCtrl22Res = 0x84,
        LOCtrl21Res = 0x85,
        LOCtrl32Res = 0x86,
        LOCtrl31Res = 0x87,
        LOCtrl42Res = 0x88,
        LOCtrl41Res = 0x89,
        ATTCtrl1Res = 0x8A,
        ATTCtrl2Res = 0x8B,
        ATTCtrl3Res = 0x8C,
        ATTCtrl4Res = 0x8D,
    };

    struct VFModuleFrame {
        quint8 frameHead = VF_FRAMEHEAD;
        quint8 cmd;
        quint8 frameLength;
        QByteArray data;
    };

    QByteArray revertFrame(const VFModuleFrame& packet);
};