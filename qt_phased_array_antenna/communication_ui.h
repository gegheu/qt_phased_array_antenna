#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_communication_module.h"
#include <iostream>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include "SerialPort.h"
#include "Tcp.h"
#include "CommManager.h"

class SerialConfigDialog;
class TcpConfigDialog;

class communication : public QMainWindow
{
    Q_OBJECT

public:
    communication(QWidget* parent = nullptr);
    ~communication();
    void initSerial();
    void initNetwork();
    /*enum QSerialPort::Parity getParity();
    qint32 getBaud();
    enum QSerialPort::StopBits getStopBits();
    enum QSerialPort::DataBits getDataBits();
    QString getName();*/
    void appendLog(const QString& str, bool isSend);
    //void isBaudRateExist();
    //void serialPortClocked(bool flag);
    //void saveINI();
    QVariantList getSerialParaList();
    QVariantList getTcpParaList();


private slots:
    void handleSerialDataReceived(const QByteArray& data);
    void handleTcpDataReceived(const QByteArray& data);
    void onSerialConfigClicked();
    void onTcpConfigClicked();
    void test(VFProtocol::VFModuleFrame data);
    //串口相关槽函数
    void handleOpenSerial();
    void handleClearLog();
    void handleClearTxData();
    void handleClearTxRx();
    void handleSendData();
    void handleRecvData(const QByteArray& data, bool hexDisplay);
    //void handleLoadINI();
    void handleOpenSerialResult(bool result, const QString& errStr);

    //TCP服务器相关的槽函数
    void onConnectClicked();
    void onSendClicked();
    void onDisconnectClicked();
    void onDataReceived(const QByteArray& data, bool hexDisplay);
    void onConnectedResult(bool result, const QString& errorInfo);
    void handleTcpClearLog();
    void handleTcpClearTxData();

private:
    Ui::CommunicationClass* ui;
    ICommunication* m_serialPort = nullptr;
    ICommunication* m_tcp = nullptr;
    CommunicationManager* m_manager = nullptr;
    SerialConfigDialog* m_serialConfigDialog = nullptr;
    TcpConfigDialog* m_tcpConfigDialog = nullptr;
    //配置参数存储
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;
    bool m_serialHexDisplay;

    QString m_tcpIp;
    int m_tcpPort;
    bool m_tcpHexDisplay;
    //串口相关成员变量
    //QTimer* m_timer = nullptr;
    QSettings* m_settings = nullptr;
    qint32 m_tx_num;
    qint32 m_rx_num;

    enum ConnectionState {
        Disconnected,
        Connecting,
        Connected
    };
    ConnectionState m_tcpConnectionState;

};

