#include "iCommunication.h"

ICommunication::ICommunication(const QString& instanceId, QObject* parent)
    : QObject(parent), m_instanceId(instanceId.isEmpty() ? QUuid::createUuid().toString() : instanceId)
{
}