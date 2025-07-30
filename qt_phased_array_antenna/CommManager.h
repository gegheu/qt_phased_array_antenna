#pragma once
#include <QObject>
#include <QMap>
#include "iprotocol.h"


class CommunicationManager : public QObject {
    Q_OBJECT
public:
    CommunicationManager(QObject* parent = nullptr);

public slots:
    // 实际串口数据入口
    void onDataReceived(const QByteArray& data);

signals:
    // 转发给UI或其他模块
    //void protocolDataParsed(const QString& protocolId, const QVariantMap& data);

private:
    QMap<qint64, IProtocol*> m_protocols; // 协议标识 -> 协议实例
};