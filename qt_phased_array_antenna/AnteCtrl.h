#pragma once
#include <QWidget>
#include <QSerialPort>
#include "ui_AnteCtrl.h"
#include "iCommunication.h"
#include "SerialConfigDialog.h"
#include "AntennaProtocol.h"

class AnteCtrl : public QWidget
{
    Q_OBJECT

public:
    AnteCtrl(QWidget* parent = nullptr);
    ~AnteCtrl();

private slots:
    // 串口控制
    void on_set_uart_clicked();           // 串口配置
    void on_open_uart_clicked();          // 打开/关闭串口
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);
    void handleSerialDataReceived(const QString& instanceId, const QByteArray& data);

    // 协议处理
    void handleAntennaEvent(const AntennaProtocol::AnteModuleFrame& frame);

    // RS422 命令发送（对应UI上的10个协议）
    void on_spkg_send_clicked();                    // 射频通道开关控制指令
    void on_dtdyx_send_clicked();                   // 单通道移相衰减控制指令
    void on_yxsjjzz_write_send_clicked();           // 移相衰减校准值写入指令
    void on_yxsjjzz_read_send_clicked();            // 移相衰减校准值读出指令
    void on_yxsjkzm_write_send_clicked();           // 移相衰减控制码写入指令
    void on_yxsjkzm_read_send_clicked();            // 移相衰减控制码读出指令
    void on_fxjqkzm_write_send_clicked();           // 幅相加权控制码写入指令
    void on_fxjqkzm_read_send_clicked();            // 幅相加权控制码读出指令
    void on_open_uart_11_clicked();                 // 波束指向控制指令

    // UI交互
    void appendLog(const QString& str, bool isSend);
    void scrollToBottom();
    void on_ante_recv_type_textChanged(const QString& text);  // 当前指令类型显示

private:
    void initSerial();
    void initProtocol();
    QVariantList getSerialParaList();

    // 各协议命令构建函数
    QByteArray buildSwitchCtrlCommand();           // 射频通道开关控制指令
    QByteArray buildAttenuatorCtrlCommand();       // 单通道移相衰减控制指令
    QByteArray buildCalibrationWriteCommand();     // 移相衰减校准值写入
    QByteArray buildCalibrationReadCommand();      // 移相衰减校准值读出
    QByteArray buildControlCodeWriteCommand();     // 移相衰减控制码写入
    QByteArray buildControlCodeReadCommand();      // 移相衰减控制码读出
    QByteArray buildWeightCodeWriteCommand();      // 幅相加权控制码写入
    QByteArray buildWeightCodeReadCommand();       // 幅相加权控制码读出
    QByteArray buildBeamSteeringCommand();         // 波束指向控制指令

    // 辅助函数
    quint8 getAntennaSelect();     // 获取天线选择（发射/接收）
    quint8 getSwitchAction();      // 获取开关动作（打开/关闭）

    Ui::AnteCtrlClass* ui;

    // 通信相关
    ICommunication* m_serialPort;
    AntennaProtocol* m_protocol;
    SerialConfigDialog* m_serialConfigDialog;

    // 串口配置
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;

    // 统计
    qint32 m_txNum;
    qint32 m_rxNum;

    // 自动滚动
    bool m_autoScroll;
    int m_maxLogLines;
};