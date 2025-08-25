#pragma once

#include <QDialog>
#include <QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QSerialPortInfo>
#include <QString> 

// ʹ��ǰ������
namespace Ui {
    class SerialConfigDialog;
}

class SerialConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialConfigDialog(QWidget* parent = nullptr);
    ~SerialConfigDialog();

    // ��ȡ���ò����ķ���
    QString portName() const;
    int baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::StopBits stopBits() const;
    bool hexDisplay() const;

    // ���õ�ǰ���õķ���
    void setCurrentConfig(const QString& port, int baud,
        QSerialPort::DataBits dataBits,
        QSerialPort::Parity parity,
        QSerialPort::StopBits stopBits,
        bool hexDisplay);

    // INI�ļ�����
    void loadINI();
    void saveINI();

signals:
    void portDisconnected(); // �˿ڶϿ��ź�

private slots:
    void refreshPorts();
    void onAccepted();

private:
    // ʹ�����������ռ��޶�
    Ui::SerialConfigDialog* ui;
    QTimer* m_refreshTimer;
    QSettings* m_settings;
    mutable QString m_lastUsedPort;
};