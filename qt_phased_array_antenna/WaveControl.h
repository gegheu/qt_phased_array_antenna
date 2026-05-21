#pragma once

#include <QWidget>
#include <QSerialPort>
#include <QVariantList>
#include "ui_WaveControl.h"
#include "iCommunication.h"
#include "SerialConfigDialog.h"

class WaveControl : public QWidget
{
    Q_OBJECT

public:
    explicit WaveControl(QWidget* parent = nullptr);
    ~WaveControl();

    // Resource injection from MainWindow.
    void setDevice(ICommunication* device);

private slots:
    void on_btnSerialSetting_clicked();
    void on_btnOpenSerial_clicked();
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);

private:
    QVariantList getSerialParaList() const;

    Ui::WaveControl* ui;

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
