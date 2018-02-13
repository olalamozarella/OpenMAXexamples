#include "CameraEncode.h"

#include <fstream>
#include <unistd.h>

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"
#include "src/core/Tunnel.h"
#include "src/components/Camera.h"
#include "src/components/EncoderH264.h"
#include "src/components/NullSink.h"
#include "src/threadworkers/FileWriter.h"

#define OUTPUT_FILENAME "test.camera.encoded.h264"
#define CAPTURING_TIME_SECONDS 5

using namespace std;

class CameraEncode::DataClass
{
public:
    DataClass() {}
    ~DataClass() {}

    Camera* camera;
    EncoderH264* encoder;
    NullSink* nullSink;

    ofstream outputFile;

    bool outputBuffersCreated;
};

CameraEncode::CameraEncode()
    : TestCase( TESTCASE_NAME_CAMERA_ENCODE )
{
    d = new DataClass();
    d->camera = new Camera();
    d->encoder = new EncoderH264();
    d->nullSink = new NullSink();
    d->outputBuffersCreated = false;
}

CameraEncode::~CameraEncode()
{
    delete d->nullSink;
    delete d->encoder;
    delete d->camera;
    delete d;
}

void CameraEncode::Init()
{
    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_Init failed" + CommonFunctions::ErrorToString( err ) );
        return;
    }
    LOG_INFO( "OMX_Init successful" );

    d->outputFile.open( OUTPUT_FILENAME, ios::out | ios::binary );
    if ( d->outputFile.is_open() == false ) {
        LOG_ERR( "Cannot open input file" );
        return;
    }
}

void CameraEncode::Run()
{
    bool ok = d->camera->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->encoder->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->nullSink->Init();
    if ( ok == false ) {
        LOG_ERR( "Error init component, destroying.." );
        return;
    }

    ok = d->camera->ConfigureFHDCapture();
    if ( ok == false ) {
        LOG_ERR( "Error configure camera" );
        return;
    }

    OMX_PARAM_PORTDEFINITIONTYPE cameraPortdef;
    ok = d->camera->GetPortDefinition( Camera::OutputVideoPort, cameraPortdef );
    if ( ok == false ) {
        LOG_ERR( "Error get camera portdef" );
        return;
    }

    ok = d->encoder->SetVideoParametersForCamera( cameraPortdef );
    if ( ok == false ) {
        LOG_ERR( "Error setting decoder params" );
        return;
    }

    Tunnel tunnelCameraNullsink( d->camera, Camera::PreviewPort, d->nullSink, NullSink::InputPort1 );
    ok = tunnelCameraNullsink.SetupTunnel();
    if ( ok == false ) {
        LOG_ERR( "Error setup tunnel" );
        return;
    }

    Tunnel tunnelCameraEncoder( d->camera, Camera::OutputVideoPort, d->encoder, EncoderH264::InputPort );
    ok = tunnelCameraEncoder.SetupTunnel();
    if ( ok == false ) {
        LOG_ERR( "Error setup tunnel" );
        return;
    }

    ok = d->camera->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }
    LOG_INFO( "Actual camera state: " + d->camera->GetComponentState() );

    ok = d->encoder->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }
    LOG_INFO( "Actual renderer state: " + d->encoder->GetComponentState() );

    ok = d->nullSink->ChangeState( OMX_StateIdle );
    if ( ok == false ) {
        LOG_ERR( "Error changing state, destroying.." );
        return;
    }
    LOG_INFO( "Actual renderer state: " + d->nullSink->GetComponentState() );

    ok = d->nullSink->EnablePort( NullSink::InputPort1 );
    if ( ok == false ) {
        LOG_ERR( "Error enabling camera output port" );
        return;
    }
    ok = d->nullSink->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, NullSink::InputPort1, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( "Error enabling camera output port - event did not come during timeout" );
        return;
    }

    ok = d->camera->EnablePort( Camera::PreviewPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling camera output port" );
        return;
    }
    ok = d->camera->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, Camera::PreviewPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( "Error enabling camera output port - event did not come during timeout" );
        return;
    }

    ok = d->camera->EnablePort( Camera::OutputVideoPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling camera output port" );
        return;
    }
    ok = d->camera->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, Camera::OutputVideoPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( "Error enabling camera output port - event did not come during timeout" );
        return;
    }

    ok = d->encoder->EnablePort( EncoderH264::InputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling renderer input port" );
        return;
    }
    ok = d->encoder->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, EncoderH264::InputPort, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( "Error enabling renderer input port - event did not come during timeout" );
        return;
    }

    ok = d->encoder->EnablePortBuffers( EncoderH264::OutputPort );
    if ( ok == false ) {
        LOG_ERR( "Error enabling output ports" );
        return;
    }
    d->outputBuffersCreated = true;

    ok = d->nullSink->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    ok = d->encoder->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    ok = d->camera->ChangeState( OMX_StateExecuting );
    if ( ok == false ) {
        LOG_ERR( "Error changing state to executing" );
        return;
    }

    ok = d->camera->StartCapturing( Camera::OutputVideoPort );
    if ( ok == false ) {
        LOG_ERR( "Error start camera capture" );
        return;
    }

    FileWriter fileWriter( d->encoder, &d->outputFile, EncoderH264::OutputPort );
    fileWriter.Start();

    for ( int i = 12; i > 0; i-- ) {
        usleep( CAPTURING_TIME_SECONDS * 1000 * 1000 );
    }

    fileWriter.Stop();

    LOG_INFO( "Finished run" );
}

void CameraEncode::Destroy()
{
    TestCase::Run();

    if ( d->outputFile.is_open() == true ) {
        d->outputFile.close();
        LOG_INFO( "output file closed" );
    }

    if ( d->outputBuffersCreated == true ) {
        bool ok = d->encoder->DisablePortBuffers( EncoderH264::OutputPort );
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
