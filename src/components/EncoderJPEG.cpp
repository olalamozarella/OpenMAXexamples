#include "EncoderJPEG.h"

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"

EncoderJPEG::EncoderJPEG()
    : Component( "OMX.broadcom.image_encode" )
{
}

bool EncoderJPEG::SetOutputImageParameters()
{
    OMX_PARAM_PORTDEFINITIONTYPE portdefInput;
    CommonFunctions::InitStructure( portdefInput );
    portdefInput.nPortIndex = InputPort;
    bool ok = GetParameter( OMX_IndexParamPortDefinition, &portdefInput );
    if ( ok == false ) {
        return false;
    }

    OMX_IMAGE_PORTDEFINITIONTYPE& imageDetailsInput = portdefInput.format.image;
    imageDetailsInput.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    imageDetailsInput.nFrameWidth = 1920;
    imageDetailsInput.nStride = 1920;
    imageDetailsInput.nFrameHeight = 1080;
    imageDetailsInput.nSliceHeight = 1088;

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

    OMX_IMAGE_PORTDEFINITIONTYPE& imageDetailsOutput = portdefOutput.format.image;
    imageDetailsOutput.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;
    imageDetailsOutput.eCompressionFormat = OMX_IMAGE_CodingJPEG;
    imageDetailsOutput.nFrameWidth = 1920;
    imageDetailsOutput.nStride = 1920; // has to be multiple of 32
    imageDetailsOutput.nFrameHeight = 1080;
    imageDetailsOutput.nSliceHeight = 1088; //rounded up to nearest multiple of 16
    ok = SetParameter( OMX_IndexParamPortDefinition, &portdefOutput );
    if ( ok == false ) {
        return false;
    }

    LOG_INFO( "Image format set successful" );

    return true;
}
