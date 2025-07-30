#pragma once
#include <QObject>
#include <QByteArray>
#include <QVariantMap>
#include<numeric>

class IProtocol : public QObject {
    Q_OBJECT
public:
    IProtocol(QObject* parent = nullptr) : QObject(parent) {};
    virtual QByteArray buildCommand(const QByteArray& data, const QVariantMap& para = {}) = 0;
    virtual void parseResponse(const QByteArray& data) = 0;
    
    enum CheckType {
        Sum = 0x01,
    };

    qint8 frameCheck(const QByteArray& data,enum CheckType type) {
        switch (type) {
        case Sum:return std::accumulate(data.begin(), data.end(), qint8(0));
        default:
            return -1;
        }
    }
};
