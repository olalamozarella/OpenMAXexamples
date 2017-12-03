#include "TunnelBufferFeeder.h"

#include "src/core/Logger.h"
#include "src/core/Component.h"
#include "src/core/CommonFunctions.h"

using namespace std;

class TunnelBufferFeeder::DataClass
{
public:
    DataClass()
    {
        producerComponent = NULL;
        producerPort = 0;
        consumerComponent = NULL;
        consumerPort = 0;
        isFinishing = NULL;
    }

    ~DataClass()
    {
    }

    Component* producerComponent;
    OMX_U32 producerPort;
    Component* consumerComponent;
    OMX_U32 consumerPort;
    bool* isFinishing;
};

TunnelBufferFeeder::TunnelBufferFeeder( Component* producerComponent, OMX_U32 producerPort, Component* consumerComponent, OMX_U32 consumerPort, bool* isFinishing )
{
    d = new DataClass();

    d->producerComponent = producerComponent;
    d->producerPort = producerPort;
    d->consumerComponent = consumerComponent;
    d->consumerPort = consumerPort;
    d->isFinishing = isFinishing;
}

TunnelBufferFeeder::~TunnelBufferFeeder()
{
    delete d;
}

bool TunnelBufferFeeder::DoSomething()
{
    if ( d->producerComponent == NULL || d->producerPort == 0 || d->consumerComponent == NULL || d->consumerPort == 0 || d->isFinishing == NULL ) {
        LOG_ERR( "Cannot start TunnelBufferFeeder - member variables are not initialized" );
        return false;
    }

    LOG_INFO_VERBOSE( "TunnelBufferFeeder - DoSomething" );

    // main loop - wait for FillBufferDone and send buffer to consumer
    bool ok = true;
    bool foundEOS = false;
    while ( ShouldFinish() == false ) {
        OMX_BUFFERHEADERTYPE* buffer = NULL;
        ok = d->producerComponent->WaitForOutputBuffer( d->producerPort, buffer );
        if ( ok == false ) {
            LOG_WARN( "No output buffer after timeout passed" );
            continue;
        }

        if ( buffer->nFlags & OMX_BUFFERFLAG_EOS ) {
            foundEOS = true;
            *d->isFinishing = true;
        }

        ok = d->consumerComponent->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "Error in consumer->EmptyThisBuffer" );
            return false;
        }

        if ( foundEOS == true ) {
            break;
        }
    }

    // change producer state to IDLE, so that every buffer sent by fillThisBuffer call is returned in FillBufferDone event
    ok = d->producerComponent->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing producer state to idle" );
        return false;
    }

    int availableBufferCount = 0;
    int allocatedBufferCount = 0;
    while ( ShouldFinish() == false ) {
        ok = d->producerComponent->GetBufferCount( d->producerPort, availableBufferCount, allocatedBufferCount );
        if ( ok == false ) {
            LOG_ERR( "Error getting producer buffer count" );
            return false;
        }

        if ( availableBufferCount == allocatedBufferCount ) {
            LOG_INFO_VERBOSE( "All buffers available" );
            break;
        }

        LOG_WARN( "Not all buffers are available: allocated=" + INT2STR( allocatedBufferCount ) + " available:" + INT2STR( availableBufferCount ) );
        OMX_BUFFERHEADERTYPE* buffer = NULL;
        ok = d->producerComponent->WaitForOutputBuffer( d->producerPort, buffer );
        if ( ok == false ) {
            LOG_ERR( "Not all buffers are available - timeout occured" );
            continue;
        } else {
            ok = d->producerComponent->AddAllocatedBufferToMap( d->producerPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
                continue;
            }
            LOG_INFO( "TunnelBufferFeeder - Adding buffer to producer collection" );
        }
    }

    LOG_INFO_VERBOSE( "TunnelBufferFeeder finishing" );
    return true;
}

