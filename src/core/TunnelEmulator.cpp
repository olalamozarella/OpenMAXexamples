#include "TunnelEmulator.h"

#include "src/core/Component.h"

class TunnelEmulator::DataClass
{
public:
    DataClass();
    ~DataClass();

    Component* producer;
    OMX_U32 producerPort;
    Component* consumer;
    OMX_U32 consumerPort;

    bool isFinishing;
};

TunnelEmulator::DataClass::DataClass()
{
    producer = NULL;
    producerPort = 0;
    consumer = NULL;
    consumerPort = 0;

    isFinishing = false;
}

TunnelEmulator::DataClass::~DataClass()
{

}

TunnelEmulator::TunnelEmulator( Component* producer, OMX_U32 producerPort, Component* consumer, OMX_U32 consumerPort )
{
    d = new DataClass();

    d->producer = producer;
    d->producerPort = producerPort;
    d->consumer = consumer;
    d->consumerPort = consumerPort;
}

TunnelEmulator::~TunnelEmulator()
{
    delete d;
}

bool TunnelEmulator::SetupTunnel()
{
    return false;
}

bool TunnelEmulator::StartTunnel()
{
    return false;
}

bool TunnelEmulator::WaitForThreadJoin()
{
    return false;
}

bool TunnelEmulator::DestroyTunnel()
{
    return false;
}
