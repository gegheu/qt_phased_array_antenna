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
    // ������زۺ���
    void on_serialConfigButton_clicked();
    void on_serialSwitch_clicked();
    void on_serialSendButton_clicked();
    void on_serialClearLogButton_clicked();
    void on_serialClearTxDataButton_clicked();
    void on_serialClearCountButton_clicked();
    void handleSerialDataReceived(const QString& instanceId, const QByteArray& data);
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);

    // TCP��زۺ���
    void on_tcpConfigButton_clicked();
    void on_tcpConnectButton_clicked();
    void on_tcpDisconnectButton_clicked();
    void on_tcpSendButton_clicked();
    void on_tcpClearLogButton_clicked();
    void on_tcpClearTxDataButton_clicked();
    void handleTcpDataReceived(const QString& instanceId, const QByteArray& data);
    void handleTcpConnectedResult(const QString& instanceId, bool result, const QString& errorInfo);

    // Э�鴦��
    void test(VFProtocol::VFModuleFrame data);

private:
    Ui::Module2* ui;

    // ������س�Ա
    ICommunication* m_serialPort;

    // TCP��س�Ա
    ICommunication* m_tcp;

    // ͨ�Ź�����
    CommunicationManager* m_manager;

    // �������ò���
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;
    bool m_serialHexDisplay;

    // TCP���ò���
    QString m_tcpIp;
    int m_tcpPort;
    bool m_tcpHexDisplay;

    // ����
    qint32 m_serialTxNum;
    qint32 m_serialRxNum;

    // TCP����״̬
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