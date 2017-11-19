#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include "src/core/Component.h"

class VideoRenderer : public Component
{
public:
    VideoRenderer();

    static const int InputPort = 90;

    bool SetRenderParameters();
};

#endif // VIDEORENDERER_H
