#include "MPEG2Stuff.h"

#include <fstream>

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/core/Tunnel.h"
#include "src/components/DecoderMPEG2.h"
#include "src/components/VideoRenderer.h"

#define FILENAME "test.mpeg2"

using namespace std;

class MPEG2Stuff::DataClass
{
public:
    DataClass();
    ~DataClass();

    DecoderMPEG2* decoder;
    VideoRenderer* renderer;

    ifstream inputFile;
    long remainingFileSize;
    bool inputBuffersCreated;

    bool ReadFileToBuffer( ifstream& inputFile, OMX_BUFFERHEADERTYPE* buffer );
};

MPEG2Stuff::DataClass::DataClass()
{
    inputBuffersCreated = false;
    remainingFileSize = 0;
}

MPEG2Stuff::DataClass::~DataClass()
{

}

bool MPEG2Stuff::DataClass::ReadFileToBuffer( ifstream& inputFile, OMX_BUFFERHEADERTYPE* buffer )
{
    if ( buffer == NULL ) {
        LOG_ERR( "NULL buffer" );
        return false;
    }

    if ( inputFile.is_open() == false ) {
        LOG_ERR( "File is not open" );
        return false;
    }

    if ( inputFile.good() == false ) {
        LOG_ERR( "input file stream is not good" );
        return false;
    }

    long bufferSize = buffer->nAllocLen;
    //LOG_INFO( "Reading file, bytes to read: " + INT2STR( bufferSize ) );

    inputFile.read( ( char* ) buffer->pBuffer, bufferSize );

    if ( inputFile.good() == true ) {
        //LOG_INFO( "File read successful" );
    } else if ( inputFile.eof() == true ) {
        LOG_WARN( "Found EOF" );
        buffer->nFlags |= OMX_BUFFERFLAG_EOS;
    } else {
        LOG_ERR( "File read error" );
    }

    long readBytes = inputFile.gcount();
    buffer->nFilledLen = readBytes;
    //LOG_INFO( "Bytes read: " + INT2STR( readBytes ) );

    return true;
}

MPEG2Stuff::MPEG2Stuff()
    : TestCase( TESTCASE_NAME_MPEG2STUFF )
{
    d = new DataClass();
    d->decoder = new DecoderMPEG2();
    d->renderer = new VideoRenderer();
}

MPEG2Stuff::~MPEG2Stuff()
{
    delete d->renderer;
    delete d->decoder;
    delete d;
}

void MPEG2Stuff::Init()
{
    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Init successful" );

    d->inputFile.open( FILENAME, ios::in | ios::binary );
    if ( d->inputFile.is_open() == false ) {
        LOG_ERR( "Cannot open input file" );
        return;
    }

    d->inputFile.seekg( 0, ios::end );
    d->remainingFileSize = d->inputFile.tellg();
    d->inputFile.seekg( 0, ios::beg );

    LOG_INFO( "File opened, size: " + INT2STR( d->remainingFileSize ) );
}

void MPEG2Stuff::Run()
{
    TestCase::Run();

    bool ok = d->decoder->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->decoder->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }

    LOG_INFO( "Actual decoder state: " + d->decoder->GetComponentState() );

    ok = d->renderer->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->renderer->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }

    LOG_INFO( "Actual renderer state: " + d->renderer->GetComponentState() );

    ok = d->decoder->SetVideoParameters();
    if ( ok == false ) {
        LOG_ERR( "Error setting decoder params" );
        return;
    }

    ok = d->decoder->EnablePortBuffers( DecoderMPEG2::InputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling port buffers" );
        return;
    }
    d->inputBuffersCreated = true;

    ok = d->decoder->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    //feed some data
    bool portSettingChangedOccured = false;
    while ( portSettingChangedOccured == false ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->decoder->GetInputBuffer( DecoderMPEG2::InputPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        ok = d->ReadFileToBuffer( d->inputFile, buffer );
        if ( ok == false ) {
            LOG_ERR( "read file failed" );
            break;
        }

        ok = d->decoder->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "empty first buffer failed" );
            break;
        }

        portSettingChangedOccured = d->decoder->WaitForEvent( OMX_EventPortSettingsChanged, DecoderMPEG2::OutputPort, 0, EVENT_HANDLER_NO_TIMEOUT );
    }

    Tunnel tunnelDecoderRenderer( d->decoder, DecoderMPEG2::OutputPort, d->renderer, VideoRenderer::InputPort );
    ok = tunnelDecoderRenderer.SetupTunnel();
    if ( ok == false ) {
        LOG_ERR( "Error setup tunnel" );
        return;
    }

    ok = d->renderer->SetRenderParameters();
    if ( ok == false ) {
        LOG_ERR( "Error set render parameters" );
        return;
    }

    ok = d->decoder->EnablePort( DecoderMPEG2::OutputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling decoder output port" );
        return;
    }

    ok = d->renderer->EnablePort( VideoRenderer::InputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling renderer input port" );
        return;
    }

    ok = d->renderer->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    while ( true ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->decoder->GetInputBuffer( DecoderMPEG2::InputPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        ok = d->ReadFileToBuffer( d->inputFile, buffer );
        if ( ok == false ) {
            LOG_ERR( "read file failed - adding buffer back to map" );
            ok = d->decoder->AddAllocatedBufferToMap( DecoderMPEG2::InputPort, buffer );
            if ( ok == false ) {
                LOG_ERR( "Cannot add allocated buffer to map manually" );
            }
            break;
        }

        ok = d->decoder->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "empty first buffer failed" );
            break;
        }
    }

    int availableCount = 0;
    int allocatedCount = 0;
    while ( true ) {
        ok = d->decoder->GetBufferCount( DecoderMPEG2::InputPort, availableCount, allocatedCount );
        if ( ok == false ) {
            LOG_ERR( "Error measuring buffer count" );
        }

        if ( availableCount == allocatedCount ) {
            LOG_INFO( "All buffers are available" );
            break;
        } else {
            LOG_WARN( "Not all buffers are available: allocated=" + INT2STR( allocatedCount ) + " available:" + INT2STR( availableCount ) );
            ok = d->decoder->WaitForEmptyBufferDone( 100 );
            if ( ok == false ) {
                LOG_ERR( "Not all buffers are available - timeout occured" );
            }
        }
    }

    LOG_INFO( "Finished run" );
}

void MPEG2Stuff::Destroy()
{
    if ( d->inputFile.is_open() == true ) {
        d->inputFile.close();
        LOG_INFO( "input file closed" );
    }

    if ( d->inputBuffersCreated == true ) {
        bool ok = d->decoder->DisablePortBuffers( DecoderMPEG2::InputPort );
        if ( ok == false ) {
            LOG_ERR( "DisablePortBuffers failed" );
        } else {
            LOG_INFO( "DisablePortBuffers successful" );
        }
    }

    OMX_ERRORTYPE err = OMX_Deinit();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Deinit failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }

    LOG_INFO( "OMX_Deinit successful" );
}
