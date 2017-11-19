#ifndef DECODERH264_H
#define DECODERH264_H

#include "src/core/Component.h"

class DecoderH264 : public Component
{
public:
    DecoderH264();

    void SetVideoParameters();
};

#endif // DECODERH264_H
