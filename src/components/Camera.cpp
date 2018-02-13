#include "Camera.h"

#include "IL/OMX_Broadcom.h"
#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"
#include "src/core/ProjectDefines.h"

const int CAM_DEVICE_NUMBER = 0;
const int VIDEO_WIDTH       = 1920;
const int VIDEO_HEIGHT      = 1080;
const int VIDEO_FRAMERATE   = 60;

Camera::Camera()
    : Component( "OMX.broadcom.camera" )
{

}

bool Camera::ConfigureFHDCapture()
{
    // Request a callback to be made when OMX_IndexParamCameraDeviceNumber is
    // changed signaling that the camera device is ready for use.
    OMX_CONFIG_REQUESTCALLBACKTYPE cbtype;
    CommonFunctions::InitStructure( cbtype );
    cbtype.nPortIndex = OMX_ALL;
    cbtype.nIndex     = OMX_IndexParamCameraDeviceNumber;
    cbtype.bEnable    = OMX_TRUE;

    OMX_ERRORTYPE err = OMX_SetConfig( GetHandle(), OMX_IndexConfigRequestCallback, &cbtype );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetConfig failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    // Set device number, this triggers the callback configured just above
    OMX_PARAM_U32TYPE device;
    CommonFunctions::InitStructure( device );
    device.nPortIndex = OMX_ALL;
    device.nU32 = CAM_DEVICE_NUMBER;
    err = OMX_SetParameter( GetHandle(), OMX_IndexParamCameraDeviceNumber, &device );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    // Configure video format emitted by camera preview output port
    OMX_PARAM_PORTDEFINITIONTYPE previewPortdef;
    CommonFunctions::InitStructure( previewPortdef );
    previewPortdef.nPortIndex = 70;
    err = OMX_GetParameter( GetHandle(), OMX_IndexParamPortDefinition, &previewPortdef );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_GetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }
    previewPortdef.format.video.nFrameWidth  = VIDEO_WIDTH;
    previewPortdef.format.video.nFrameHeight = VIDEO_HEIGHT;
    previewPortdef.format.video.xFramerate   = VIDEO_FRAMERATE << 16;
    // Stolen from gstomxvideodec.c of gst-omx
    previewPortdef.format.video.nStride      = ( previewPortdef.format.video.nFrameWidth + previewPortdef.nBufferAlignment - 1 ) & ( ~( previewPortdef.nBufferAlignment - 1 ) );
    previewPortdef.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    err = OMX_SetParameter( GetHandle(), OMX_IndexParamPortDefinition, &previewPortdef );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    OMX_PARAM_PORTDEFINITIONTYPE videoCapturePortdef;
    CommonFunctions::InitStructure( videoCapturePortdef );
    videoCapturePortdef.nPortIndex = 70;
    err = OMX_GetParameter( GetHandle(), OMX_IndexParamPortDefinition, &videoCapturePortdef );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_GetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }
    videoCapturePortdef.nPortIndex = 71;
    err = OMX_SetParameter( GetHandle(), OMX_IndexParamPortDefinition, &videoCapturePortdef );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }
    OMX_CONFIG_FRAMERATETYPE framerate;
    CommonFunctions::InitStructure( framerate );
    framerate.xEncodeFramerate = previewPortdef.format.video.xFramerate;
    framerate.nPortIndex = Camera::PreviewPort;
    err = OMX_SetConfig( GetHandle(), OMX_IndexConfigVideoFramerate, &framerate );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetConfig failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }
    framerate.nPortIndex = Camera::OutputVideoPort;
    err = OMX_SetConfig( GetHandle(), OMX_IndexConfigVideoFramerate, &framerate );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetConfig failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    bool ok = WaitForEvent( OMX_EventParamOrConfigChanged, 0xFFFFFFFF, OMX_IndexParamCameraDeviceNumber, EVENT_HANDLER_TIMEOUT_MS_MAX );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Camera device number event did not come" );
        return false;
    }

    return true;
}

bool Camera::StartCapturing( const int capturingPort )
{
    // Set capturing port to video
    OMX_CONFIG_PORTBOOLEANTYPE capture;
    CommonFunctions::InitStructure( capture );
    capture.nPortIndex = capturingPort;
    capture.bEnabled = OMX_TRUE;
    OMX_ERRORTYPE err = OMX_SetParameter( GetHandle(), OMX_IndexConfigPortCapturing, &capture );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }
    return true;
}

bool Camera::StopCapturing( const int capturingPort )
{
    // Set capturing port to video
    OMX_CONFIG_PORTBOOLEANTYPE capture;
    CommonFunctions::InitStructure( capture );
    capture.nPortIndex = capturingPort;
    capture.bEnabled = OMX_FALSE;
    OMX_ERRORTYPE err = OMX_SetParameter( GetHandle(), OMX_IndexConfigPortCapturing, &capture );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }
    return true;
}
