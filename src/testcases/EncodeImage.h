#ifndef ENCODEIMAGE_H
#define ENCODEIMAGE_H

#include "TestCase.h"

class EncodeImage : public TestCase
{
public:
    EncodeImage();
    virtual ~EncodeImage();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // ENCODEIMAGE_H
