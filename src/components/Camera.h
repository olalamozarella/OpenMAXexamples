#ifndef CAMERA_H
#define CAMERA_H

#include "src/core/Component.h"

class Camera : public Component
{
public:
    Camera();

    static const int InputPort = 73;
    static const int PreviewPort = 70;
    static const int OutputVideoPort = 71;
    static const int OutputImagePort = 72;

    bool ConfigureFHDCapture();
    bool StartCapturing( const int capturingPort );
    bool StopCapturing( const int capturingPort );
};

#endif // CAMERA_H
