#pragma once
#include <QObject>
#include <QMap>
#include "iprotocol.h"


class CommunicationManager : public QObject {
    Q_OBJECT
public:
    CommunicationManager(QObject* parent = nullptr);

public slots:
    // ʵ�ʴ����������
    void onDataReceived(const QByteArray& data);

signals:
    // ת����UI������ģ��
    //void protocolDataParsed(const QString& protocolId, const QVariantMap& data);

private:
    QMap<qint64, IProtocol*> m_protocols; // Э���ʶ -> Э��ʵ��
};