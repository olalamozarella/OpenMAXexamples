#ifndef VIDEODECODERENDER_H
#define VIDEODECODERENDER_H

#include "TestCase.h"

class VideoDecodeRender : public TestCase
{
public:
    VideoDecodeRender();
    ~VideoDecodeRender();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // VIDEODECODERENDER_H
