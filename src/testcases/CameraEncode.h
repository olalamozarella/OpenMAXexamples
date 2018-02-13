#ifndef CAMERAENCODE_H
#define CAMERAENCODE_H

#include "TestCase.h"

class CameraEncode : public TestCase
{
public:
    CameraEncode();
    virtual ~CameraEncode();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // CAMERAENCODE_H
