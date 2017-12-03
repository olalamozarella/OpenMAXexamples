#include "FileWriter.h"

#include "src/core/Logger.h"
#include "src/core/Component.h"
#include "src/core/CommonFunctions.h"
#include "src/core/ProjectDefines.h"

using namespace std;

class FileWriter::DataClass
{
public:
    DataClass()
    {
        port = 0;
        component = NULL;
        outputFile = NULL;
    }

    ~DataClass()
    {
    }

    Component* component;
    ofstream* outputFile;
    OMX_U32 port;
};

FileWriter::FileWriter( Component* component, std::ofstream* outputFile, OMX_U32 port )
    : ThreadWorker()
{
    d = new DataClass();

    d->component = component;
    d->outputFile = outputFile;
    d->port = port;
}

FileWriter::~FileWriter()
{
    delete d;
}

bool FileWriter::DoSomething()
{
    if ( d->component == NULL || d->outputFile == NULL ) {
        LOG_ERR( "Cannot start fileWriter - NULL member variables" );
        return false;
    }

    LOG_INFO_VERBOSE( "FileWriter - DoSomething" );

    int availableBufferCount = 0;
    int allocatedBufferCount = 0;
    bool ok = d->component->GetBufferCount( d->port, availableBufferCount, allocatedBufferCount );
    if ( ok == false ) {
        LOG_ERR( "Error measuring buffer count" );
        return false;
    }
    for ( int i = 0; i < availableBufferCount; i++ ) {
        LOG_INFO_VERBOSE( "Taking buffer for fill" );

        OMX_BUFFERHEADERTYPE* buffer = NULL;
        ok = d->component->TakeOutputBufferFromCollection( d->port, buffer );
        if ( ok == false ) {
            LOG_ERR( "Error takeOutputBufferFromCollection" );
            return false;
        }

        ok = d->component->FillThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "Error fillThisBuffer" );
            return false;
        }
    }

    bool foundEOS = false;
    ok = true;
    while ( ShouldFinish() == false ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->component->WaitForOutputBuffer( d->port, buffer );
        if ( ok == false ) {
            LOG_WARN( "No available output buffer after timeout passed" );
            continue;
        }

        if ( buffer->nFlags & OMX_BUFFERFLAG_EOS ) {
            foundEOS = true;
        }

        ok = CommonFunctions::WriteBufferToFile( *d->outputFile, buffer );
        if ( ok == false ) {
            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
            LOG_ERR( "read file failed - adding buffer back to map" );
            ok = d->component->AddAllocatedBufferToMap( d->port, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        if ( foundEOS == true ) {
            LOG_INFO_VERBOSE( "EOS found" );
            ok = d->component->AddAllocatedBufferToMap( d->port, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        } else {
            d->component->FillThisBuffer( buffer );
        }

    }

    // set state to idle
    ok = d->component->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error setting component to Idle state" );
    }

    // wait for all buffers to be returned
    while ( ShouldFinish() == false ) {
        ok = d->component->GetBufferCount( d->port, availableBufferCount, allocatedBufferCount );
        if ( ok == false ) {
            LOG_ERR( "Error measuring buffer count" );
            break;
        }

        if ( availableBufferCount == allocatedBufferCount ) {
            LOG_INFO_VERBOSE( "All buffers are available" );
            break;
        } else {
            LOG_WARN( "Not all buffers are available: allocated=" + INT2STR( allocatedBufferCount ) + " available:" + INT2STR( availableBufferCount ) );
            OMX_BUFFERHEADERTYPE* buffer = NULL;
            ok = d->component->WaitForOutputBuffer( d->port, buffer );
            if ( ok == false ) {
                LOG_ERR( "Not all buffers are available - timeout occured" );
                continue;
            } else {
                ok = d->component->AddAllocatedBufferToMap( d->port, buffer );
                if ( ok == false ) {
                    LOG_ERR( "Cannot add allocated buffer to map manually" );
                }
            }
        }
    }

    LOG_INFO_VERBOSE( "FileWriter finishing" );
    return true;
}

