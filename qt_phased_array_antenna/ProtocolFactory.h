#pragma once
#include "VFProtocol.h"

class ProtocolFactory {
public:
    virtual ~ProtocolFactory() = default;
    virtual IProtocol* createProtocol() = 0;
};

class VFProtocolFactory : public ProtocolFactory {
public:
    IProtocol* createProtocol() override {
        return new VFProtocol();
    }
};

class AntennaProtocolFactory : public ProtocolFactory {
public:
    IProtocol* createProtocol() override {
        return new AntennaProtocol();
    }
};