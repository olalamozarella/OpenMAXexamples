#ifndef DECODERMPEG2_H
#define DECODERMPEG2_H

#include "src/core/Component.h"

class DecoderMPEG2 : public Component
{
public:
    DecoderMPEG2();

    static const int InputPort = 130;
    static const int OutputPort = 131;

    bool SetVideoParameters();
};

#endif // DECODERMPEG2_H
