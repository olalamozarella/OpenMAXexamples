#ifndef TUNNELBUFFERREDEEMER_H
#define TUNNELBUFFERREDEEMER_H

#include "src/core/ThreadWorker.h"
#include "IL/OMX_Core.h"

class Component;

class TunnelBufferRedeemer : public ThreadWorker
{
public:
    TunnelBufferRedeemer( Component* producerComponent, OMX_U32 producerPort, Component* consumerComponent, OMX_U32 consumerPort, bool* isFinishing );
    virtual ~TunnelBufferRedeemer();

    virtual bool DoSomething();

private:
    class DataClass;
    DataClass* d;
};

#endif // TUNNELBUFFERREDEEMER_H
