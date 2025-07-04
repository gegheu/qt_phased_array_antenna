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

class communication : public QMainWindow
{
    Q_OBJECT

public:
    communication(QWidget* parent = nullptr);
    ~communication();
    void serial_init();
    void network_init();
    enum QSerialPort::Parity getParity();
    //enum QSerialPort::BaudRate getBaud();
    qint32 getBaud();
    enum QSerialPort::StopBits getStopBits();
    enum QSerialPort::DataBits getDataBits();
    QString getName();
    void appendLog(const QString &str, bool isSend);

private slots:
    //串口相关槽函数
    void refreshPorts();
    void handleOpenSerial();
    void handleClearLog();
    void handleClearTxData();
    void handleClearTxRx();
    void handleSendData();
    void handleRecvData();
    void handleSaveINI();
    void handleLoadINI();

    
    //TCP服务器相关的槽函数
    void onConnectClicked();
    void onSendClicked();
    void onDisconnectClicked();
    void onConnected();
    void onDisconnected();
    void onDataReceived();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    Ui::CommunicationClass* ui;
    //串口相关成员变量
    QTimer* m_timer = nullptr;
    QSerialPort* m_serialport = nullptr;
    QSettings* m_settings=nullptr;
    qint32 m_tx_num;
    qint32 m_rx_num;
   
    //网口相关成员变量
    QTcpSocket* m_tcpSocket = nullptr;
    bool m_isConnected;
};

