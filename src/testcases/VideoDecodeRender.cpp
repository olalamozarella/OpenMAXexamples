#include "VideoDecodeRender.h"

#include <cstdio>
#include <fstream>

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/core/Tunnel.h"
#include "src/components/DecoderH264.h"
#include "src/components/VideoRenderer.h"
#include "src/threadworkers/FileReader.h"

#define FILENAME "test.camera.encoded.h264"

using namespace std;

class VideoDecodeRender::DataClass
{
public:
    DataClass();
    ~DataClass();

    DecoderH264* decoder;
    VideoRenderer* renderer;

    ifstream inputFile;
    long remainingFileSize;
    bool inputBuffersCreated;
};

VideoDecodeRender::DataClass::DataClass()
{
    inputBuffersCreated = false;
    remainingFileSize = 0;
}

VideoDecodeRender::DataClass::~DataClass()
{
}

VideoDecodeRender::VideoDecodeRender()
    : TestCase( TESTCASE_NAME_VIDEO_DECODE_RENDER )
{
    d = new DataClass();
    d->decoder = new DecoderH264();
    d->renderer = new VideoRenderer();
}

VideoDecodeRender::~VideoDecodeRender()
{
    delete d->renderer;
    delete d->decoder;
    delete d;
}

void VideoDecodeRender::Init()
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

void VideoDecodeRender::Run()
{
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

    ok = d->decoder->EnablePortBuffers( DecoderH264::InputPort );
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
    bool foundEOF = false;
    while ( portSettingChangedOccured == false ) {
        OMX_BUFFERHEADERTYPE* buffer;
        ok = d->decoder->WaitForInputBuffer( DecoderH264::InputPort, buffer );
        if ( ( ok == false ) || ( buffer == NULL ) ) {
            LOG_ERR( "Error get input buffer" );
            break;
        }

        ok = CommonFunctions::ReadFileToBuffer( d->inputFile, buffer, foundEOF );
        if ( ok == false ) {
            LOG_ERR( "read file failed" );
            break;
        }

        ok = d->decoder->EmptyThisBuffer( buffer );
        if ( ok == false ) {
            LOG_ERR( "empty first buffer failed" );
            break;
        }

        portSettingChangedOccured = d->decoder->WaitForEvent( OMX_EventPortSettingsChanged, DecoderH264::OutputPort, 0, EVENT_HANDLER_NO_TIMEOUT );
    }

    d->decoder->GetVideoParameters( d->decoder->OutputPort );
    d->renderer->GetVideoParameters( d->renderer->InputPort );

    Tunnel tunnelDecoderRenderer( d->decoder, DecoderH264::OutputPort, d->renderer, VideoRenderer::InputPort );
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

    ok = d->decoder->EnablePort( DecoderH264::OutputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling decoder output port" );
        return;
    }

    ok = d->decoder->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, DecoderH264::OutputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( "Error enabling renderer input port - event did not come during timeout" );
        return;
    }

    ok = d->renderer->EnablePort( VideoRenderer::InputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling renderer input port" );
        return;
    }

    ok = d->renderer->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, VideoRenderer::InputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( "Error enabling renderer input port - event did not come during timeout" );
        return;
    }

    ok = d->renderer->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    d->decoder->GetVideoParameters( d->decoder->OutputPort );
    d->renderer->GetVideoParameters( d->renderer->InputPort );

    FileReader fileReader( d->decoder, &d->inputFile, d->decoder->InputPort );
    fileReader.Start();
    fileReader.WaitForThreadJoin();

    ok = d->decoder->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing decoder state to idle" );
        return;
    }

    ok = d->renderer->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing renderer state to idle" );
        return;
    }

    LOG_INFO( "Finished run" );
}

void VideoDecodeRender::Destroy()
{
    if ( d->inputFile.is_open() == true ) {
        d->inputFile.close();
        LOG_INFO( "input file closed" );
    }

    if ( d->inputBuffersCreated == true ) {
        bool ok = d->decoder->DisablePortBuffers( DecoderH264::InputPort );
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
