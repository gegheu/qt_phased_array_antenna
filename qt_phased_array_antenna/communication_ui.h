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

class communication : public QMainWindow
{
    Q_OBJECT

public:
    communication(QWidget* parent = nullptr);
    ~communication();
    void initSerial();
    void initNetwork();
    enum QSerialPort::Parity getParity();
    qint32 getBaud();
    enum QSerialPort::StopBits getStopBits();
    enum QSerialPort::DataBits getDataBits();
    QString getName();
    void appendLog(const QString &str, bool isSend);
    void isBaudRateExist();
    void serialPortClocked(bool flag);
    void saveINI();
    QVariantList getParaList();

private slots:
    //串口相关槽函数
    void refreshPorts();
    void handleOpenSerial();
    void handleClearLog();
    void handleClearTxData();
    void handleClearTxRx();
    void handleSendData();
    void handleRecvData(const QByteArray& data);
    void handleLoadINI();
    void handleOpenSerialResult(bool result, const QString& errStr);

    //TCP服务器相关的槽函数
    void onConnectClicked();
    void onSendClicked();
    void onDisconnectClicked();
    void onDataReceived(const QByteArray& data);
    void onConnectedResult(bool result, const QString& errorInfo);

private:
    Ui::CommunicationClass* ui;
    ICommunication* m_serialPort = nullptr;
    ICommunication* m_tcp = nullptr;
    //串口相关成员变量
    QTimer* m_timer = nullptr;
    QSettings* m_settings=nullptr;
    qint32 m_tx_num;
    qint32 m_rx_num;
};

