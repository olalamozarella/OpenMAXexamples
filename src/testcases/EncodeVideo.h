#ifndef ENCODEVIDEO_H
#define ENCODEVIDEO_H

#include "TestCase.h"

class EncodeVideo : public TestCase
{
public:
    EncodeVideo();
    ~EncodeVideo();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // ENCODEVIDEO_H
