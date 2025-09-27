#include "SerialPortManager.h"
#include <QDebug>

SerialPortManager* SerialPortManager::m_instance = nullptr;

SerialPortManager& SerialPortManager::instance()
{
    if (!m_instance) {
        m_instance = new SerialPortManager();
    }
    return *m_instance;
}

SerialPortManager::SerialPortManager(QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &SerialPortManager::refreshPorts);
}

SerialPortManager::~SerialPortManager()
{
    stopMonitoring();
}

QStringList SerialPortManager::availablePorts() const
{
    return m_currentPorts;
}

void SerialPortManager::startMonitoring(int intervalMs)
{
    
    refreshPorts();

    
    m_timer->start(intervalMs);
}

void SerialPortManager::stopMonitoring()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void SerialPortManager::refreshPorts()
{
    QStringList newPorts;

    try {
        for (const auto& port : QSerialPortInfo::availablePorts()) {
            newPorts << QString(port.portName() + " " + port.description());
        }
    }
    catch (...) {
        qWarning() << "Error while enumerating serial ports";
        return;
    }

    // ÅÅÐò
    newPorts.sort();

    if (newPorts != m_currentPorts) {
        m_currentPorts = newPorts;
        emit portsChanged(m_currentPorts);
    }
}