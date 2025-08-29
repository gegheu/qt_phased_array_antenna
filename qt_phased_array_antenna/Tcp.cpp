#include "Tcp.h"

Tcp::Tcp(QObject* parent) : ICommunication(parent)
{
    socket = new QTcpSocket(this);
    m_timeoutTimer = new QTimer(this); // ������ʱ��ʱ��
    m_timeoutTimer->setSingleShot(true); // ���δ���
    connect(m_timeoutTimer, &QTimer::timeout, this, &Tcp::handleTimeout);
    connect(socket, &QTcpSocket::readyRead, this, &ICommunication::readData);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        m_timeoutTimer->stop();
        emit connectStatus(true, "no error");
        });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        m_timeoutTimer->stop();
        emit connectStatus(false, QStringLiteral("�����ѶϿ�"));
        });

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
        [this](QAbstractSocket::SocketError error) {
            m_timeoutTimer->stop();
            QString errorStr;
            switch (error) {
            case QAbstractSocket::ConnectionRefusedError:
                errorStr = QStringLiteral("���ӱ��ܾ�");
                break;
            case QAbstractSocket::RemoteHostClosedError:
                errorStr = QStringLiteral("Զ�������ر�����");
                break;
            case QAbstractSocket::HostNotFoundError:
                errorStr = QStringLiteral("����δ�ҵ�");
                break;
            case QAbstractSocket::SocketAccessError:
                errorStr = QStringLiteral("�׽��ַ��ʴ���");
                break;
            case QAbstractSocket::SocketResourceError:
                errorStr = QStringLiteral("�׽�����Դ����");
                break;
            case QAbstractSocket::SocketTimeoutError:
                errorStr = QStringLiteral("���ӳ�ʱ");
                break;
            case QAbstractSocket::NetworkError:
                errorStr = QStringLiteral("�������");
                break;
            default:
                errorStr = QStringLiteral("δ֪����: %1").arg(error);
                break;
            }
            emit connectStatus(false, errorStr);
        });
}

Tcp::~Tcp()
{
    if (isConnected()) {
        doDisconnect();
    }
}

void Tcp::portConnect(const QVariantList& params)
{
    // ȷ��û�����ڽ��е�����
    if (isConnected() || isConnecting()) {
        return;
    }

    socket->connectToHost(params.at(0).toString(), params.at(1).toInt());
    m_timeoutTimer->start(5000);
}

void Tcp::doDisconnect()
{
    m_timeoutTimer->stop();
    if (isConnecting()) {
        socket->abort();

        emit connectStatus(false, QStringLiteral("�ֶ��Ͽ�"));
    }
    else {
        socket->disconnectFromHost();
        // ���ﲻ��������״̬�źţ��ȴ� disconnected �ź�
    }
}

bool Tcp::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

bool Tcp::isConnecting() const
{
    return socket->state() == QAbstractSocket::ConnectingState;
}

qint64 Tcp::writeData(const QByteArray& data)
{
    return socket->write(data);
}

QIODevice* Tcp::getDevice()
{
    return socket;
}

void Tcp::handleTimeout()
{
    if (isConnecting()) {
        socket->abort(); // ��ֹ����
        emit connectStatus(false, QStringLiteral("���ӳ�ʱ"));
    }
}

void Tcp::abortConnection()
{
    m_timeoutTimer->stop();
    if (isConnecting()) {
        socket->abort();
        emit connectStatus(false, QStringLiteral("������ȡ��"));
    }
}