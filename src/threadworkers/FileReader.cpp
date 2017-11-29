#include "FileReader.h"

#include "src/core/Logger.h"
#include "src/core/Component.h"
#include "src/core/CommonFunctions.h"

using namespace std;

class FileReader::DataClass
{
public:
    DataClass()
    {
        workingPort = 0;
        component = NULL;
        inputFile = NULL;
    }

    ~DataClass()
    {
    }

    Component* component;
    ifstream* inputFile;
    OMX_U32 workingPort;
};

FileReader::FileReader( Component* component, ifstream* inputFile, OMX_U32 port )
    : ThreadWorker()
{
    d = new DataClass();

    d->component = component;
    d->inputFile = inputFile;
    d->workingPort = port;
}

FileReader::~FileReader()
{
    delete d;
}

bool FileReader::DoSomething()
{
    if ( d->component == NULL || d->inputFile == NULL )
    {
        LOG_ERR( "Cannot start fileReader - NULL member variables" );
        return false;
    }

    LOG_INFO( "FileReader - DoSomething" );

    bool foundEOF = false;
    bool ok = true;
    while ( ShouldFinish() == false ) {

        // Wait for available input buffer from component port-buffer collection
        //  Buffer will be removed from component port-buffer collection!
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->component->WaitForInputBuffer( d->workingPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        // Read file and save it to buffer. Set EOF flag if end of file is reached.
        ok = CommonFunctions::ReadFileToBuffer( *d->inputFile, buffer, foundEOF );
        if ( ok == false ) {
            // If reading fails, buffer is still empty and should be returned to component port-buffer collection.
            LOG_ERR( "read file failed - adding buffer back to map" );
            ok = d->component->AddAllocatedBufferToMap( d->workingPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        // Buffer is filled => call EmptyThisBuffer
        ok = d->component->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "empty first buffer failed" );
            break;
        }

        // EndOfFile reached - break loop
        if ( foundEOF == true ) {
            break;
        }
    }

    // Wait until all buffers are returned to component port-buffer collection (all buffers are processed)
    int availableCount = 0;
    int allocatedCount = 0;
    while ( ShouldFinish() == false ) {
        ok = d->component->GetBufferCount( d->workingPort, availableCount, allocatedCount );
        if ( ok == false ) {
            LOG_ERR( "Error measuring buffer count" );
            break;
        }

        if ( availableCount == allocatedCount ) {
            LOG_INFO( "All buffers are available" );
            break;
        } else {
            LOG_WARN( "Not all buffers are available: allocated=" + INT2STR( allocatedCount ) + " available:" + INT2STR( availableCount ) );
            ok = d->component->WaitForBufferEvent( d->workingPort, 100 );
            if ( ok == false ) {
                LOG_ERR( "Not all buffers are available - timeout occured" );
            }
        }
    }

    // File is read, all buffers are processed - finish. Buffer cleanup is done by FileReader's owner.
    LOG_INFO( "FileReader - finishing executing" );
    return true;
}

