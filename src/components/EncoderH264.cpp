#include "EncoderH264.h"

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"

EncoderH264::EncoderH264()
    : Component( "OMX.broadcom.video_encode" )
{
}

bool EncoderH264::SetVideoParameters()
{
    OMX_VIDEO_PARAM_PORTFORMATTYPE videoformat;
    CommonFunctions::InitStructure( videoformat );
    videoformat.nPortIndex = OutputPort;
    videoformat.eCompressionFormat = OMX_VIDEO_CodingAVC;

    bool ok = SetParameter( OMX_IndexParamVideoPortFormat, &videoformat );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error setting video parameters" );
        return false;
    }

    return true;
}
