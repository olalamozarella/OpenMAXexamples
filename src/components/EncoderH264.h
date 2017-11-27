#ifndef ENCODERH264_H
#define ENCODERH264_H

#include "src/core/Component.h"

class EncoderH264 : public Component
{
public:
    EncoderH264();

    static const int InputPort = 200;
    static const int OutputPort = 201;
};

#endif // ENCODERH264_H
