#ifndef TUNNELBUFFERFEEDER_H
#define TUNNELBUFFERFEEDER_H

#include "src/core/ThreadWorker.h"
#include "IL/OMX_Core.h"

class Component;

class TunnelBufferFeeder : public ThreadWorker
{
public:
    TunnelBufferFeeder(Component* producerComponent, OMX_U32 producerPort, Component* consumerComponent, OMX_U32 consumerPort , bool *isFinishing );
    virtual ~TunnelBufferFeeder();

    virtual bool DoSomething();

private:
    class DataClass;
    DataClass* d;
};

#endif // TUNNELBUFFERFEEDER_H
