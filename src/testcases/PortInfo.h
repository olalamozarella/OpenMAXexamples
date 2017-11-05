#ifndef PORTINFO_H
#define PORTINFO_H

#include "TestCase.h"

class OMX_PARAM_PORTDEFINITIONTYPE;

class PortInfo : public TestCase
{
public:
    PortInfo();
    virtual ~PortInfo();

    void Run();

private:
    void GetPortInfo( string componentName, int portNumber );
    void PrintPortInfo( OMX_PARAM_PORTDEFINITIONTYPE* portdef );
};

#endif // PORTINFO_H
