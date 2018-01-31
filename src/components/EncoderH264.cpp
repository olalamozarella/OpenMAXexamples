#include "EncoderH264.h"

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"

EncoderH264::EncoderH264()
    : Component( "OMX.broadcom.video_encode" )
{
}

bool EncoderH264::SetVideoParameters()
{
    OMX_PARAM_PORTDEFINITIONTYPE portdefInput;
    CommonFunctions::InitStructure( portdefInput );
    portdefInput.nPortIndex = InputPort;
    bool ok = GetParameter( OMX_IndexParamPortDefinition, &portdefInput );
    if ( ok == false ) {
        return false;
    }

    OMX_VIDEO_PORTDEFINITIONTYPE& videoDetailsInput = portdefInput.format.video;
    videoDetailsInput.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    videoDetailsInput.nFrameWidth = 1920;
    videoDetailsInput.nStride = 1920; // has to be multiple of 32
    videoDetailsInput.nFrameHeight = 1088;
    videoDetailsInput.nSliceHeight = 1088; //rounded up to nearest multiple of 16
    ok = SetParameter( OMX_IndexParamPortDefinition, &portdefInput );
    if ( ok == false ) {
        return false;
    }

    OMX_PARAM_PORTDEFINITIONTYPE portdefOutput;
    CommonFunctions::InitStructure( portdefOutput );
    portdefOutput.nPortIndex = OutputPort;
    ok = GetParameter( OMX_IndexParamPortDefinition, &portdefOutput );
    if ( ok == false ) {
        return false;
    }

    OMX_VIDEO_PORTDEFINITIONTYPE& videoDetailsOutput = portdefOutput.format.video;
    videoDetailsOutput.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    videoDetailsOutput.eCompressionFormat = OMX_VIDEO_CodingAVC;
    videoDetailsOutput.nFrameWidth = 1920;
    videoDetailsOutput.nStride = 1920; // has to be multiple of 32
    videoDetailsOutput.nFrameHeight = 1088;
    videoDetailsOutput.nSliceHeight = 1088; //rounded up to nearest multiple of 16
    ok = SetParameter( OMX_IndexParamPortDefinition, &portdefOutput );
    if ( ok == false ) {
        return false;
    }

//    OMX_VIDEO_PARAM_PORTFORMATTYPE videoformat;
//    CommonFunctions::InitStructure( videoformat );
//    videoformat.nPortIndex = OutputPort;
//    videoformat.eCompressionFormat = OMX_VIDEO_CodingAVC;
//    videoformat.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;

//    ok = SetParameter( OMX_IndexParamVideoPortFormat, &videoformat );
//    if ( ok == false ) {
//        LOG_ERR( GetComponentName() + ":Error setting video parameters" );
//        return false;
//    }

    return true;
}
