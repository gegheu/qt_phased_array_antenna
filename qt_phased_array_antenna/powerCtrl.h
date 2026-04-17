#pragma once
#include <QWidget>
#include <QSerialPort>
#include "ui_powerCtrl.h"
#include "iCommunication.h"
#include "SerialPort.h"
#include "SerialConfigDialog.h"
#include "PowerProtocol.h"

class powerCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit powerCtrl(QWidget* parent = nullptr);
    ~powerCtrl();

    void setDevice(ICommunication* device);
    void setProtocol(PowerProtocol* proto);

private slots:
    // 串口控制
    void on_set_uart_clicked();           // 串口配置
    void on_open_uart_clicked();          // 打开/关闭串口
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);

    // 电源状态处理：业务逻辑响应
    void handlePowerStatus(const PowerProtocol::PowerStatusFrame& status);

    // 电源通道控制 - 12V通道
    void on_open12v1_clicked();
    void on_close12v1_clicked();
    void on_open12v2_clicked();
    void on_close12v2_clicked();
    void on_open12v3_clicked();
    void on_close12v3_clicked();
    void on_open12v4_clicked();
    void on_close12v4_clicked();

    // 电源通道控制 - 24V通道
    void on_open24v1_clicked();
    void on_close24v1_clicked();
    void on_open24v2_clicked();
    void on_close24v2_clicked();

    // 全部控制
    void on_openall_clicked();
    void on_closeall_clicked();

private:
    QVariantList getSerialParaList();
    void updateUIDisplay(int channel, double voltage, double current);

    // 构建控制数据
    QByteArray buildControlData(int channel, bool open);
    QByteArray buildControlDataAll(bool open);

    Ui::powerCtrlClass* ui;

    // 通信相关（由外部注入）
    ICommunication* m_serialPort;
    PowerProtocol* m_protocol;
    SerialConfigDialog* m_serialConfigDialog;

    // 串口配置
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;

    // 电源通道编号
    enum PowerChannel {
        CH_12V1 = 0,
        CH_12V2 = 1,
        CH_12V3 = 2,
        CH_12V4 = 3,
        CH_24V1 = 4,
        CH_24V2 = 5
    };
};
