#ifndef BUFFERSNOIL_H
#define BUFFERSNOIL_H

#include "TestCase.h"

class BuffersNoIL : public TestCase
{
public:
    BuffersNoIL();
    ~BuffersNoIL();

    void Run();
    void Init();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // BUFFERSNOIL_H
