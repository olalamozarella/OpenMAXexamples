#include "DecoderJPEG.h"

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"

DecoderJPEG::DecoderJPEG()
    : Component( "OMX.broadcom.image_decode" )
{
}

bool DecoderJPEG::SetImageParameters()
{
    OMX_IMAGE_PARAM_PORTFORMATTYPE imagePortFormat;
    CommonFunctions::InitStructure( imagePortFormat );

    imagePortFormat.nPortIndex = InputPort;
    imagePortFormat.eCompressionFormat = OMX_IMAGE_CodingJPEG;

    bool ok = SetParameter( OMX_IndexParamImagePortFormat, &imagePortFormat );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error setting image parameters" );
        return false;
    }

    LOG_INFO( "Image format set successful" );

    return true;
}
