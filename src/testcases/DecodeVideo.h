#ifndef DECODEVIDEO_H
#define DECODEVIDEO_H

#include "TestCase.h"

class DecodeVideo : public TestCase
{
public:
    DecodeVideo();
    virtual ~DecodeVideo();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // DECODEVIDEO_H
