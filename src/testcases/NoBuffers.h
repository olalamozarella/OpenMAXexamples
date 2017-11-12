#ifndef NOBUFFERS_H
#define NOBUFFERS_H

#include "TestCase.h"

class NoBuffers : public TestCase
{
public:
    NoBuffers();
    virtual ~NoBuffers();

    void Run();

private:
    class DataClass;
    DataClass* d;
};

#endif // NOBUFFERS_H
