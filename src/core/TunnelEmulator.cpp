#include "TunnelEmulator.h"

#include <list>
#include "src/core/Component.h"
#include "src/core/Logger.h"
#include "src/threadworkers/TunnelBufferFeeder.h"
#include "src/threadworkers/TunnelBufferRedeemer.h"

using namespace std;

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

    list<OMX_BUFFERHEADERTYPE*> usedBuffers;
    TunnelBufferFeeder* feederThread;
    TunnelBufferRedeemer* redeemerThread;
};

TunnelEmulator::DataClass::DataClass()
{
    producer = NULL;
    producerPort = 0;
    consumer = NULL;
    consumerPort = 0;

    isFinishing = false;

    feederThread = NULL;
    redeemerThread = NULL;
}

TunnelEmulator::DataClass::~DataClass()
{
    if ( feederThread != NULL ) {
        delete feederThread;
        feederThread = 0;
    }

    if ( redeemerThread != NULL ) {
        delete redeemerThread;
        redeemerThread = 0;
    }
}

TunnelEmulator::TunnelEmulator( Component* producer, OMX_U32 producerPort, Component* consumer, OMX_U32 consumerPort )
{
    d = new DataClass();

    d->producer = producer;
    d->producerPort = producerPort;
    d->consumer = consumer;
    d->consumerPort = consumerPort;

    d->feederThread = new TunnelBufferFeeder( d->producer, d->producerPort, d->consumer, d->consumerPort, &d->isFinishing );
    d->redeemerThread = new TunnelBufferRedeemer( d->producer, d->producerPort, d->consumer, d->consumerPort, &d->isFinishing );
}

TunnelEmulator::~TunnelEmulator()
{
    delete d;
}

bool TunnelEmulator::SetupTunnel()
{
    bool ok = d->producer->EnablePortBuffers( d->producerPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling producer port buffers" );
        return false;
    }

    return true;
}

bool TunnelEmulator::StartTunnel()
{
    bool ok = d->feederThread->Start();
    if ( ok == false ) {
        LOG_ERR( "Error starting feeder thread" );
        return false;
    }

    ok = d->redeemerThread->Start();
    if ( ok == false ) {
        LOG_ERR( "Error starting redeemer thread" );
        return false;
    }

    return true;
}

bool TunnelEmulator::WaitForThreadJoin()
{
    return false;
}

bool TunnelEmulator::DestroyTunnel()
{
    return false;
}
