#ifndef MODULE2_H
#define MODULE2_H

#include <QDialog>
#include <QSerialPort>
#include "iCommunication.h"
#include "CommManager.h"
#include "SerialConfigDialog.h"
#include "TcpConfigDialog.h"

namespace Ui {
    class Module2;
}

class Module2 : public QDialog
{
    Q_OBJECT

public:
    explicit Module2(QWidget* parent = nullptr);
    ~Module2();

private slots:
    // 串口相关槽函数
    void on_serialConfigButton_clicked();
    void on_serialSwitch_clicked();
    void on_serialSendButton_clicked();
    void on_serialClearLogButton_clicked();
    void on_serialClearTxDataButton_clicked();
    void on_serialClearCountButton_clicked();
    void handleSerialDataReceived(const QString& instanceId, const QByteArray& data);
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);

    // TCP相关槽函数
    void on_tcpConfigButton_clicked();
    void on_tcpConnectButton_clicked();
    void on_tcpDisconnectButton_clicked();
    void on_tcpSendButton_clicked();
    void on_tcpClearLogButton_clicked();
    void on_tcpClearTxDataButton_clicked();
    void handleTcpDataReceived(const QString& instanceId, const QByteArray& data);
    void handleTcpConnectedResult(const QString& instanceId, bool result, const QString& errorInfo);

    // 协议处理
    void test(VFProtocol::VFModuleFrame data);

private:
    Ui::Module2* ui;

    // 串口相关成员
    ICommunication* m_serialPort;

    // TCP相关成员
    ICommunication* m_tcp;

    // 通信管理器
    CommunicationManager* m_manager;

    // 串口配置参数
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;
    bool m_serialHexDisplay;

    // TCP配置参数
    QString m_tcpIp;
    int m_tcpPort;
    bool m_tcpHexDisplay;

    // 计数
    qint32 m_serialTxNum;
    qint32 m_serialRxNum;

    // TCP连接状态
    enum TcpConnectionState {
        TcpDisconnected,
        TcpConnecting,
        TcpConnected
    };
    TcpConnectionState m_tcpConnectionState;

    void initSerial();
    void initTcp();
    QVariantList getSerialParaList();
    QVariantList getTcpParaList();
    void appendSerialLog(const QString& str, bool isSend);
    void appendTcpLog(const QString& str, bool isSend);
};

#endif // MODULE2_H