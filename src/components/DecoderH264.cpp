#include "DecoderH264.h"

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"

DecoderH264::DecoderH264()
    : Component( "OMX.broadcom.video_decode" )
{
}

bool DecoderH264::SetVideoParameters()
{
    OMX_VIDEO_PARAM_PORTFORMATTYPE videoformat;
    CommonFunctions::InitStructure( videoformat );
    videoformat.nPortIndex = InputPort;
    videoformat.eCompressionFormat = OMX_VIDEO_CodingAVC;

    bool ok = SetParameter( OMX_IndexParamVideoPortFormat, &videoformat );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error setting video parameters" );
        return false;
    }

    return true;
}
