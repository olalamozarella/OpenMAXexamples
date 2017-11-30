#ifndef DECODERJPEG_H
#define DECODERJPEG_H

#include "src/core/Component.h"

/**
 * @brief The DecoderJPEG class - wrapper for OMX.broadcom.image_decode with JPEG input format.
 * @note Component creates just single output buffer with whole decoded raw image data. This can be problem for bigger JPEG input files -
 *  If alllocated outputBuffer would be too big, OMX_UseBuffer will fail with OMX_ErrorInsufficientResources.
 *
 * Execution steps:
 * - load JPEG file
 * - load part of input file to input buffer
 * - call EmptyThisBuffer, wait for PortSettingsChanged event
 * - allocate and enable output buffer (minimal output buffer size in portdef is changed after PortSettingsChanged event!)
 * - run threadWorkers
 */
class DecoderJPEG : public Component
{
public:
    DecoderJPEG();

    static const int InputPort = 320;
    static const int OutputPort = 321;

    bool SetImageParameters();
};

#endif // DECODERJPEG_H
