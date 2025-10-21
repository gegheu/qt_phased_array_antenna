#pragma once
#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QSerialPortInfo>
#include <QStringList>

class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    static SerialPortManager& instance();

    QStringList availablePorts() const;
    void startMonitoring(int intervalMs = 1000);
    void stopMonitoring();

signals:
    void portsChanged(const QStringList& ports);

private:
    explicit SerialPortManager(QObject* parent = nullptr);
    ~SerialPortManager();

    void refreshPorts();

private:
    QTimer* m_timer;
    QStringList m_currentPorts;
    static SerialPortManager* m_instance;
};

#endif // SERIALPORTMANAGER_H