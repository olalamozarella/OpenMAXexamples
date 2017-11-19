#ifndef DECODERH264_H
#define DECODERH264_H

#include "src/core/Component.h"

class DecoderH264 : public Component
{
public:
    DecoderH264();

    static const int InputPort = 130;
    static const int OutputPort = 131;

    bool SetVideoParameters();
};

#endif // DECODERH264_H
