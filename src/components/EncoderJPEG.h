#ifndef ENCODERJPEG_H
#define ENCODERJPEG_H

#include "src/core/Component.h"

class EncoderJPEG : public Component
{
public:
    EncoderJPEG();

    static const int InputPort = 340;
    static const int OutputPort = 341;

    bool SetOutputImageParameters();
};

#endif // ENCODERJPEG_H
