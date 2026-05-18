#pragma once

#include <QWidget>
#include <QSerialPort>
#include <QVariantList>
#include <QTimer>
#include "ui_LEOTrack.h"
#include "iCommunication.h"
#include "SerialConfigDialog.h"

class LEOTrackPage : public QWidget
{
    Q_OBJECT

public:
    explicit LEOTrackPage(QWidget* parent = nullptr);
    ~LEOTrackPage();

    // Inject communication device from MainWindow.
    void setDevice(ICommunication* device);

private slots:
    void on_btnBrowseFile_clicked();
    void on_btnClearFile_clicked();
    void on_btnRealtime_clicked();
    void on_btnPreset_clicked();
    void on_btnSerialConfig_clicked();
    void on_btnOpenSerial_clicked();
    void handleOpenSerialResult(const QString& instanceId, bool result, const QString& errStr);
    void updateBeijingTime();

private:
    QVariantList getSerialParaList() const;
    bool loadEphFile(const QString& filePath);

    Ui::TaskTrajectoryDisplay* ui;

    // Communication resources (injected externally).
    ICommunication* m_serialPort;
    SerialConfigDialog* m_serialConfigDialog;

    // Serial parameters.
    QString m_serialPortName;
    qint32 m_serialBaudRate;
    QSerialPort::DataBits m_serialDataBits;
    QSerialPort::Parity m_serialParity;
    QSerialPort::StopBits m_serialStopBits;

    // Local Beijing time refresh timer.
    QTimer* m_beijingTimeTimer;
};
