#include "ImageDecodeRender.h"

#include <fstream>

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/core/Tunnel.h"
#include "src/components/DecoderJPEG.h"
#include "src/components/VideoRenderer.h"

#define FILENAME "test.jpeg"

using namespace std;

class ImageDecodeRender::DataClass
{
public:
    DataClass();
    ~DataClass();

    DecoderJPEG* decoder;
    VideoRenderer* renderer;

    ifstream inputFile;
    long remainingFileSize;
    bool inputBuffersCreated;
};

ImageDecodeRender::DataClass::DataClass()
{
    inputBuffersCreated = false;
    remainingFileSize = 0;
}

ImageDecodeRender::DataClass::~DataClass()
{
}

ImageDecodeRender::ImageDecodeRender()
    : TestCase( TESTCASE_NAME_IMAGE_DECODE_RENDER )
{
    d = new DataClass();
    d->decoder = new DecoderJPEG();
    d->renderer = new VideoRenderer();
}

ImageDecodeRender::~ImageDecodeRender()
{
    delete d->renderer;
    delete d->decoder;
    delete d;
}

void ImageDecodeRender::Init()
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

void ImageDecodeRender::Run()
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

    ok = d->decoder->SetImageFormat();
    if ( ok == false ) {
        LOG_ERR( "Error setting decoder params" );
        return;
    }

    ok = d->decoder->EnablePortBuffers( DecoderJPEG::InputPort );
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

    OMX_BUFFERHEADERTYPE* buffer;
    ok = d->decoder->WaitForInputBuffer( DecoderJPEG::InputPort, buffer );
    if ( ( ok == false ) || ( buffer == NULL ) ) {
        LOG_ERR( "Error get input buffer" );
        return;
    }

    bool foundEOF = false;
    ok = CommonFunctions::ReadFileToBuffer( d->inputFile, buffer, foundEOF );
    if ( ok == false ) {
        LOG_ERR( "read file failed" );
        return;
    }

    ok = d->decoder->EmptyThisBuffer( buffer );
    if ( ok == false ) {
        LOG_ERR( "empty first buffer failed" );
        return;
    }

    ok = d->decoder->WaitForEvent( OMX_EventPortSettingsChanged, DecoderJPEG::OutputPort, 0, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false )
    {
        LOG_ERR( "Port settings changed event did not come during timeout" );
        return;
    }

//    ok = d->decoder->EnablePort( DecoderJPEG::OutputPort );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling decoder output port" );
//        return;
//    }

//    ok = d->decoder->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, DecoderJPEG::OutputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling decoder output port - event did not come during timeout" );
//        return;
//    }

//    ok = d->renderer->EnablePort( VideoRenderer::InputPort );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling renderer input port" );
//        return;
//    }

//    ok = d->renderer->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, VideoRenderer::InputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
//    if ( ok == false ) {
//        LOG_ERR( "Error enabling renderer input port - event did not come during timeout" );
//        return;
//    }

    Tunnel tunnelDecoderRenderer( d->decoder, DecoderJPEG::OutputPort, d->renderer, VideoRenderer::InputPort );
    ok = tunnelDecoderRenderer.SetupTunnel();
    if ( ok == false ) {
        LOG_ERR( "Error setup tunnel" );
        return;
    }

    ok = d->renderer->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    while ( true ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->decoder->WaitForInputBuffer( DecoderJPEG::InputPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        ok = CommonFunctions::ReadFileToBuffer( d->inputFile, buffer, foundEOF );
        if ( ok == false ) {
            LOG_ERR( "read file failed - adding buffer back to map" );
            ok = d->decoder->AddAllocatedBufferToMap( DecoderJPEG::InputPort, buffer );
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
        ok = d->decoder->GetBufferCount( DecoderJPEG::InputPort, availableCount, allocatedCount );
        if ( ok == false ) {
            LOG_ERR( "Error measuring buffer count" );
        }

        if ( availableCount == allocatedCount ) {
            LOG_INFO( "All buffers are available" );
            break;
        } else {
            LOG_WARN( "Not all buffers are available: allocated=" + INT2STR( allocatedCount ) + " available:" + INT2STR( availableCount ) );
            ok = d->decoder->WaitForBufferEvent( DecoderJPEG::InputPort, 100 );
            if ( ok == false ) {
                LOG_ERR( "Not all buffers are available - timeout occured" );
            }
        }
    }

    LOG_INFO( "Finished run" );
}

void ImageDecodeRender::Destroy()
{
    if ( d->inputFile.is_open() == true ) {
        d->inputFile.close();
        LOG_INFO( "input file closed" );
    }

    if ( d->inputBuffersCreated == true ) {
        bool ok = d->decoder->DisablePortBuffers( DecoderJPEG::InputPort );
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
