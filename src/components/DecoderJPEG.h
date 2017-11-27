#ifndef DECODERJPEG_H
#define DECODERJPEG_H

#include "src/core/Component.h"

class DecoderJPEG : public Component
{
public:
    DecoderJPEG();

    static const int InputPort = 320;
    static const int OutputPort = 321;

    bool SetImageFormat();
};

#endif // DECODERJPEG_H
