#include "SerialPort.h"

SerialPort::SerialPort(QObject* parent) : ICommunication(parent)
{
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &ICommunication::readData);
    connect(serial, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        if (error == QSerialPort::ResourceError) {
            emit connectStatus(false, serial->errorString());
        }
        });
}

SerialPort::~SerialPort()
{
    if (isConnected()) {
        doDisconnect();
    }
}

void SerialPort::portConnect(const QVariantList& params)
{
    serial->setPortName(params.at(0).toString());
    serial->setBaudRate(params.at(1).toInt());
    serial->setDataBits(static_cast<QSerialPort::DataBits>(params.at(2).toInt()));
    serial->setParity(static_cast<QSerialPort::Parity>(params.at(3).toInt()));
    serial->setStopBits(static_cast<QSerialPort::StopBits>(params.at(4).toInt()));

    if (!serial->open(QSerialPort::ReadWrite)) {
        emit connectStatus(false, serial->errorString());
    }
    else {
        emit connectStatus(true, "no error");
    }
}

void SerialPort::doDisconnect()
{
    serial->close();
}

bool SerialPort::isConnected() const
{
    return serial->isOpen();
}


qint64 SerialPort::writeData(const QByteArray& data)
{
    return serial->write(data);
}

QIODevice* SerialPort::getDevice()
{
    return serial;
}