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

    // 资源注入接口：由 MainWindow 调用
    void setDevice(ICommunication* device);
    void setProtocol(AntennaProtocol* proto);

private slots:
    // 串口控制
    void on_set_uart_clicked();           // 串口配置
    void on_open_uart_clicked();          // 打开/关闭串口
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);

    // 协议处理：对应协议解析完成后的业务更新
    void onDataUpdated(const AntennaProtocol::AnteModuleFrame& frame);

    // RS422 命令发送
    void on_spkg_send_clicked();
    void on_dtdyx_send_clicked();
    void on_yxsjjzz_write_send_clicked();
    void on_yxsjjzz_read_send_clicked();
    void on_yxsjkzm_write_send_clicked();
    void on_yxsjkzm_read_send_clicked();
    void on_fxjqkzm_write_send_clicked();
    void on_fxjqkzm_read_send_clicked();
    void on_open_uart_11_clicked();

    // UI交互
    void appendLog(const QString& str, bool isSend);
    void scrollToBottom();

private:
    QVariantList getSerialParaList();

    // 各协议命令构建函数
    QByteArray buildSwitchCtrlCommand();
    QByteArray buildAttenuatorCtrlCommand();
    QByteArray buildCalibrationWriteCommand();
    QByteArray buildCalibrationReadCommand();
    QByteArray buildControlCodeWriteCommand();
    QByteArray buildControlCodeReadCommand();
    QByteArray buildWeightCodeWriteCommand();
    QByteArray buildWeightCodeReadCommand();
    QByteArray buildBeamSteeringCommand();

    // 辅助函数
    quint8 getAntennaSelect();
    quint8 getSwitchAction();

    Ui::AnteCtrlClass* ui;

    // 通信相关（由外部注入）
    ICommunication* m_serialPort;
    AntennaProtocol* m_protocol;
    SerialConfigDialog* m_serialConfigDialog;

    // 串口配置
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;

    // 统计与日志
    qint32 m_txNum;
    qint32 m_rxNum;
    bool m_autoScroll;
    int m_maxLogLines;
};