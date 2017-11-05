#ifndef PORTINFO_H
#define PORTINFO_H

#include "TestCase.h"
#include "IL/OMX_Core.h"

class PortInfo : public TestCase
{
public:
    PortInfo();
    virtual ~PortInfo();

    void Run();

private:
    bool GetComponentInfo( string componentName, OMX_HANDLETYPE* handle, OMX_VERSIONTYPE* specVersion );
    void PrintPortInfo( OMX_HANDLETYPE& handle, int portNumber );

    void PrintAllVideoPortsInfo( OMX_HANDLETYPE& handle, OMX_VERSIONTYPE& specVersion );
};

#endif // PORTINFO_H
