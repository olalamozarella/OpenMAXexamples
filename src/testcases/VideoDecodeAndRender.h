#ifndef VIDEODECODEANDRENDER_H
#define VIDEODECODEANDRENDER_H

#include "TestCase.h"

class VideoDecodeAndRender : public TestCase
{
public:
    VideoDecodeAndRender();
    ~VideoDecodeAndRender();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // VIDEODECODEANDRENDER_H
