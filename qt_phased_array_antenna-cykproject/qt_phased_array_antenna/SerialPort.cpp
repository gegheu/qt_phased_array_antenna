#include "SerialPort.h"

SerialPort::SerialPort(QObject* parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &SerialPort::onReadyRead);
}

SerialPort::~SerialPort()
{
    if (serial->isOpen()) {
        serial->close();
    }
    delete serial;
}

bool SerialPort::open(const QString &portName, int baudRate, const QSerialPort::Parity &parity, const QSerialPort::DataBits &dataBits, const QSerialPort::StopBits &stopBits)
{
    serial->setPortName(portName);
    serial->setBaudRate(baudRate);
    serial->setDataBits(dataBits);
    serial->setParity(parity);
    serial->setStopBits(stopBits);

    if (!serial->open(QSerialPort::ReadWrite)) {
        return false;
    }
    return true;
}

void SerialPort::close()
{
    if (serial->isOpen()) {
        serial->close();
    }
}

bool SerialPort::isOpen()
{
    return serial->isOpen();
}

void SerialPort::clear()
{
    if (serial->isOpen()) {
        serial->clear();
    }
}


qint64 SerialPort::write(const QByteArray& data)
{
    if (serial->isOpen()) {
        return serial->write(data);
    }
}

void SerialPort::onReadyRead()
{
    QByteArray data = serial->readAll();
    while (serial->waitForReadyRead(10)) {
        data += serial->readAll();
    }
    emit dataReceived(data);
}