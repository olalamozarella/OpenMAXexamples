#include "CameraRender.h"

#include <unistd.h>

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"
#include "src/core/Tunnel.h"
#include "src/components/Camera.h"
#include "src/components/VideoRenderer.h"
#include "src/components/NullSink.h"

#define CAPTURING_TIME_SECONDS 5

class CameraRender::DataClass
{
public:
    DataClass() {}
    ~DataClass() {}

    Camera* camera;
    VideoRenderer* renderer;
    NullSink* nullSink;
};

CameraRender::CameraRender()
    : TestCase( TESTCASE_NAME_CAMERA_RENDER )
{
    d = new DataClass();
    d->camera = new Camera();
    d->renderer = new VideoRenderer();
    d->nullSink = new NullSink();
}

CameraRender::~CameraRender()
{
    delete d->nullSink;
    delete d->renderer;
    delete d->camera;
    delete d;
}

void CameraRender::Init()
{
    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Init successful" );
}

void CameraRender::Run()
{
    TestCase::Run();

    bool ok = d->camera->Init();
    ASSERT( ok == false, "Error init component, destroying.." );

    ok = d->renderer->Init();
    ASSERT( ok == false, "Error init component, destroying.." );

    ok = d->nullSink->Init();
    ASSERT( ok == false, "Error init component, destroying.." );

    ok = d->camera->ConfigureFHDCapture();
    ASSERT( ok == false, "Error configure camera" );

    ok = d->camera->ChangeState( OMX_StateIdle );
    ASSERT( ok == false, "Error changing state, destroying.." );
    LOG_INFO( "Actual camera state: " + d->camera->GetComponentState() );

    ok = d->renderer->ChangeState( OMX_StateIdle );
    ASSERT( ok == false, "Error changing state, destroying.." );
    LOG_INFO( "Actual renderer state: " + d->renderer->GetComponentState() );

    ok = d->nullSink->ChangeState( OMX_StateIdle );
    ASSERT( ok == false, "Error changing state, destroying.." );
    LOG_INFO( "Actual renderer state: " + d->nullSink->GetComponentState() );

    Tunnel tunnelCameraNullsink( d->camera, Camera::PreviewPort, d->nullSink, NullSink::InputPort1 );
    ok = tunnelCameraNullsink.SetupTunnel();
    ASSERT( ok == false, "Error setup tunnel" );

    Tunnel tunnelCameraRenderer( d->camera, Camera::OutputVideoPort, d->renderer, VideoRenderer::InputPort );
    ok = tunnelCameraRenderer.SetupTunnel();
    ASSERT( ok == false, "Error setup tunnel" );

    ok = d->nullSink->EnablePort( NullSink::InputPort1 );
    ASSERT( ok == false, "Error enabling nullsink input port" );
    ok = d->nullSink->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, NullSink::InputPort1, EVENT_HANDLER_TIMEOUT_MS_MAX );
    ASSERT( ok == false, "Error enabling nullsink input port - event did not come during timeout" );

    ok = d->camera->EnablePort( Camera::OutputVideoPort );
    ASSERT( ok == false, "Error enabling camera output port" );
    ok = d->camera->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, Camera::OutputVideoPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    ASSERT( ok == false, "Error enabling camera output port - event did not come during timeout" );

    ok = d->renderer->EnablePort( VideoRenderer::InputPort );
    ASSERT( ok == false, "Error enabling renderer input port" );
    ok = d->renderer->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, VideoRenderer::InputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    ASSERT( ok == false, "Error enabling renderer input port - event did not come during timeout" );

    ok = d->nullSink->ChangeState( OMX_StateExecuting );
    ASSERT( ok == false, "Error changing state to executing" );

    ok = d->renderer->ChangeState( OMX_StateExecuting );
    ASSERT( ok == false, "Error changing state to executing" );

    ok = d->camera->ChangeState( OMX_StateExecuting );
    ASSERT( ok == false, "Error changing state to executing" );

    ok = d->camera->StartCapturing( Camera::OutputVideoPort );
    ASSERT( ok == false, "Error start capturing" );

    /* LOOP */
    LOG_INFO_VERBOSE( "Arrived at loop" );
    usleep( CAPTURING_TIME_SECONDS * 1000 * 1000 );
    LOG_INFO_VERBOSE( "Wake up" );

    /* FINISHING RUN */
    ok = d->camera->StopCapturing( Camera::OutputVideoPort );
    ASSERT( ok == false, "Error stop capturing" );

    ok = d->camera->ChangeState( OMX_StateIdle );
    ASSERT( ok == false, "Error changing camera state to idle" );

    ok = d->renderer->ChangeState( OMX_StateIdle );
    ASSERT( ok == false, "Error changing renderer state to idle" );

    ok = d->nullSink->ChangeState( OMX_StateIdle );
    ASSERT( ok == false, "Error changing nullsink state to idle" );

    LOG_INFO( "Finished run" );
}

void CameraRender::Destroy()
{
    OMX_ERRORTYPE err = OMX_Deinit();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Deinit failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Deinit successful" );
}
