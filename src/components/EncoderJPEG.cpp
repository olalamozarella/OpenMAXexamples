#include "EncoderJPEG.h"

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"

EncoderJPEG::EncoderJPEG()
    : Component( "OMX.broadcom.image_encode" )
{
}

bool EncoderJPEG::SetOutputImageParameters()
{
    OMX_IMAGE_PARAM_PORTFORMATTYPE imagePortFormat;
    CommonFunctions::InitStructure( imagePortFormat );

    imagePortFormat.nPortIndex = OutputPort;
    imagePortFormat.eCompressionFormat = OMX_IMAGE_CodingJPEG;

    bool ok = SetParameter( OMX_IndexParamImagePortFormat, &imagePortFormat );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error setting image parameters" );
        return false;
    }

    LOG_INFO( "Image format set successful" );

    return true;
}
