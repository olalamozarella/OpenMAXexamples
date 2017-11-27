#ifndef MPEG2STUFF_H
#define MPEG2STUFF_H

#include "TestCase.h"

class MPEG2Stuff : public TestCase
{
public:
    MPEG2Stuff();
    ~MPEG2Stuff();

    void Init();
    void Run();
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif //MPEG2STUFF_H
