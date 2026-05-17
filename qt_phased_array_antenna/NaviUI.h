#pragma once

#include <QWidget>
#include <QSerialPort>
#include <QVariantList>
#include "ui_NaviReceiver.h"
#include "iCommunication.h"
#include "SerialConfigDialog.h"

class NaviUI : public QWidget
{
    Q_OBJECT

public:
    explicit NaviUI(QWidget* parent = nullptr);
    ~NaviUI();

    // Resource injection from MainWindow.
    void setDevice(ICommunication* device);

private slots:
    void on_set_uart_clicked();
    void on_open_uart_clicked();
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);

private:
    QVariantList getSerialParaList() const;

    Ui::NaviReceiver* ui;

    // Communication resources (injected externally).
    ICommunication* m_serialPort;
    SerialConfigDialog* m_serialConfigDialog;

    // Serial parameters.
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;
};
