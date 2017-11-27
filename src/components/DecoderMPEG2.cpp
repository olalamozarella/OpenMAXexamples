#include "DecoderMPEG2.h"

#include "src/core/Logger.h"
#include "src/core/CommonFunctions.h"

DecoderMPEG2::DecoderMPEG2()
    : Component( "OMX.broadcom.video_decode" )
{
}

bool DecoderMPEG2::SetVideoParameters()
{
    OMX_VIDEO_PARAM_PORTFORMATTYPE videoformat;
    CommonFunctions::InitStructure( videoformat );
    videoformat.nPortIndex = InputPort;
    videoformat.eCompressionFormat = OMX_VIDEO_CodingMPEG2;

    // Vypise chybu errorBadParameter - netusim preco?
    // Pokial je compression format AVC, vsetko ide v pohode..
    bool ok = SetParameter( OMX_IndexParamVideoPortFormat, &videoformat );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error setting video parameters" );
        return false;
    }

    return true;
}

