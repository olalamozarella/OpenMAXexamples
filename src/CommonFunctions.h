#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <string>
#include "IL/OMX_Core.h"

class CommonFunctions
{
public:
    static std::string ErrorToString( int errorID );
    static std::string GetComponentState( OMX_HANDLETYPE handle );
    static bool DisableAllPorts( OMX_HANDLETYPE handle );

private:
    CommonFunctions();
    static bool DisableSomePorts( OMX_HANDLETYPE handle, OMX_INDEXTYPE indexType );
};

#endif // COMMONFUNCTIONS_H
