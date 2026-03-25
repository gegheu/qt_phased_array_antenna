#pragma once

#include <QWidget>
#include <QSerialPort>
#include "ui_variableFreq.h"
#include "iCommunication.h"
#include "SerialPort.h"
#include "SerialConfigDialog.h"
#include "VFProtocol.h"

class variableFreq : public QWidget
{
    Q_OBJECT

public:
    explicit variableFreq(QWidget* parent = nullptr);
    ~variableFreq();

private slots:
    // 串口控制
    void on_serialConfigButton_clicked();
    void on_serialSwitch_clicked();
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);
    void handleSerialDataReceived(const QString& instanceId, const QByteArray& data);

    // 发送控制
    void on_serialSendButton_clicked();

    // 清空控制
    void on_serialClearLogButton_clicked();
    void on_serialClearCountButton_clicked();

    // 协议响应处理
    void handleVarFreqResponse(const VFProtocol::VarFreqResponse& response);

private:
    void initSerial();
    void initProtocol();
    void initComboBox();

    void appendSerialLog(const QString& str, bool isSend);
    void updateSerialCounters(qint64 sent, qint64 received);

    QVariantList getSerialParaList();

    // 构建变频率命令
    QByteArray buildVarFreqCommand(quint8 cmd, quint32 freqMHz);
    bool getAndValidateFrequency(quint8 cmd, quint32& freqMHz);

    Ui::variableFreq* ui;

    // 通信相关
    ICommunication* m_serialPort;
    VFProtocol* m_protocol;
    SerialConfigDialog* m_serialConfigDialog;

    // 串口配置
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;

    // 计数器
    qint32 m_serialTxNum;
    qint32 m_serialRxNum;
};