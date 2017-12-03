#include "TunnelBufferRedeemer.h"

#include "src/core/Logger.h"
#include "src/core/Component.h"
#include "src/core/CommonFunctions.h"

using namespace std;

class TunnelBufferRedeemer::DataClass
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

TunnelBufferRedeemer::TunnelBufferRedeemer( Component* producerComponent, OMX_U32 producerPort, Component* consumerComponent, OMX_U32 consumerPort , bool* isFinishing )
    : ThreadWorker()
{
    d = new DataClass();

    d->producerComponent = producerComponent;
    d->producerPort = producerPort;
    d->consumerComponent = consumerComponent;
    d->consumerPort = consumerPort;
    d->isFinishing = isFinishing;
}

TunnelBufferRedeemer::~TunnelBufferRedeemer()
{
    delete d;
}

bool TunnelBufferRedeemer::DoSomething()
{
    if ( d->producerComponent == NULL || d->producerPort == 0 || d->consumerComponent == NULL || d->consumerPort == 0 || d->isFinishing == NULL ) {
        LOG_ERR( "Cannot start TunnelBufferRedeemer - member variables are not initialized" );
        return false;
    }

    LOG_INFO_VERBOSE( "TunnelBufferRedeemer - DoSomething" );

    bool ok = true;
    while ( ShouldFinish() == false ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->consumerComponent->WaitForInputBuffer( d->consumerPort, buffer );
        if ( ok == false ) {
            LOG_WARN( "No available input buffer after timeout passed" );
            continue;
        }

        if ( *d->isFinishing == true ) {
            break;
        }

        ok = d->producerComponent->FillThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "Error during producer-fillThisBuffer" );
            return false;
        }
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
        ok = d->consumerComponent->WaitForInputBuffer( d->consumerPort, buffer );
        if ( ok == false ) {
            LOG_ERR( "Not all buffers are available - timeout occured" );
            continue;
        } else {
            ok = d->producerComponent->AddAllocatedBufferToMap( d->producerPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
                continue;
            }
            LOG_INFO( "TunnelBufferRedeemer - Adding buffer to producer collection" );
        }
    }

    LOG_INFO_VERBOSE( "TunnelBufferRedeemer finishing" );
    return true;
}

