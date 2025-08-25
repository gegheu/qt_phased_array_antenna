#pragma once

#include <QDialog>
#include <QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QSerialPortInfo>
#include <QString> 

// 使用前置声明
namespace Ui {
    class SerialConfigDialog;
}

class SerialConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialConfigDialog(QWidget* parent = nullptr);
    ~SerialConfigDialog();

    // 获取配置参数的方法
    QString portName() const;
    int baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::StopBits stopBits() const;
    bool hexDisplay() const;

    // 设置当前配置的方法
    void setCurrentConfig(const QString& port, int baud,
        QSerialPort::DataBits dataBits,
        QSerialPort::Parity parity,
        QSerialPort::StopBits stopBits,
        bool hexDisplay);

    // INI文件操作
    void loadINI();
    void saveINI();

signals:
    void portDisconnected(); // 端口断开信号

private slots:
    void refreshPorts();
    void onAccepted();

private:
    // 使用完整命名空间限定
    Ui::SerialConfigDialog* ui;
    QTimer* m_refreshTimer;
    QSettings* m_settings;
    mutable QString m_lastUsedPort;
};