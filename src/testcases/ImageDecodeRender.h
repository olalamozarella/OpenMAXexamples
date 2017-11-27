#ifndef IMAGEDECODERENDER_H
#define IMAGEDECODERENDER_H

#include "TestCase.h"

class ImageDecodeRender : public TestCase
{
public:
    ImageDecodeRender();
    ~ImageDecodeRender();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // IMAGEDECODERENDER_H
