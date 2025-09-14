#ifndef COMMUNICATIONWINDOW_H
#define COMMUNICATIONWINDOW_H

#include <QMainWindow>
#include "SerialConfigDialog.h"
#include "TcpConfigDialog.h"
#include "SerialPort.h"
#include "Tcp.h"
#include "CommManager.h"

// 前置声明不同的UI类
namespace Ui {
    class CommunicationClass1;
    class CommunicationClass2;
    class CommunicationClass3;
    class CommunicationClass4;
    class CommunicationClass5;
}

class CommunicationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommunicationWindow(int windowId, QWidget* parent = nullptr);
    ~CommunicationWindow();

    int windowId() const { return m_windowId; }

signals:
    void windowClosed(int windowId);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void test(VFProtocol::VFModuleFrame data);
    void onSerialConfigClicked();
    void onTcpConfigClicked();
    void handleOpenSerial();
    void handleClearLog();
    void handleClearTxData();
    void handleClearTxRx();
    void handleSendData();
    void handleRecvData(const QByteArray& data, bool hexDisplay);
    void handleOpenSerialResult(bool result, const QString& errStr);
    void onConnectClicked();
    void onSendClicked();
    void onDisconnectClicked();
    void onDataReceived(const QByteArray& data, bool hexDisplay);
    void onConnectedResult(bool result, const QString& errorInfo);
    void handleTcpClearLog();
    void handleTcpClearTxData();
    void handleSerialDataReceived(const QByteArray& data);
    void handleTcpDataReceived(const QByteArray& data);

private:
    void initSerial();
    void initNetwork();
    void setupConnections();
    QVariantList getSerialParaList();
    QVariantList getTcpParaList();
    void appendLog(const QString& str, bool isSend);

    int m_windowId;

    // 使用void指针存储UI，因为每个窗口的UI类不同
    void* m_ui;

    ICommunication* m_serialPort = nullptr;
    ICommunication* m_tcp = nullptr;
    CommunicationManager* m_manager = nullptr;
    SerialConfigDialog* m_serialConfigDialog = nullptr;
    TcpConfigDialog* m_tcpConfigDialog = nullptr;

    // 配置参数存储
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;
    bool m_serialHexDisplay;

    QString m_tcpIp;
    int m_tcpPort;
    bool m_tcpHexDisplay;

    // 串口相关成员变量
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

#endif // COMMUNICATIONWINDOW_H