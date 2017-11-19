#include "VideoRenderer.h"

VideoRenderer::VideoRenderer()
    : Component( "OMX.broadcom.video_render" )
{
}

bool VideoRenderer::SetRenderParameters()
{
    return false;
}

