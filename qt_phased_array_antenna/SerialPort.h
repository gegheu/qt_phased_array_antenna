#pragma once
#include <QObject>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    SerialPort(QObject* parent = nullptr);
    ~SerialPort();

    bool open(const QString& portName, int baudRate, const QSerialPort::Parity& parity, const QSerialPort::DataBits& dataBits, const QSerialPort::StopBits& stopBits);
    void close();
    void clear();
    bool isOpen();
    qint64 write(const QByteArray& data);

signals:
    void dataReceived(const QByteArray& data);

private slots:
    void onReadyRead();

private:
    QSerialPort* serial;
};
