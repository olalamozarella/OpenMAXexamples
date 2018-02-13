#ifndef CAMERARENDER_H
#define CAMERARENDER_H

#include "TestCase.h"

class CameraRender : public TestCase
{
public:
    CameraRender();
    virtual ~CameraRender();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // CAMERARENDER_H
