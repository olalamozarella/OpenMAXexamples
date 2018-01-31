#include "VideoRenderer.h"

#include "src/core/CommonFunctions.h"
#include "src/core/Logger.h"

VideoRenderer::VideoRenderer()
    : Component( "OMX.broadcom.video_render" )
{
}

bool VideoRenderer::SetRenderParameters()
{
    return true;
}
