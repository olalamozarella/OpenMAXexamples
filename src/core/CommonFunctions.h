#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <string>
#include "IL/OMX_Component.h"

#define INT2STR CommonFunctions::IntToString

class CommonFunctions
{
public:
    static std::string ErrorToString( int errorID );
    static std::string StateToString( OMX_STATETYPE state );

    static std::string GetComponentState( OMX_HANDLETYPE handle );
    static bool DisableAllPorts( OMX_HANDLETYPE handle );

    static void InitStructure(OMX_PORT_PARAM_TYPE &structure );
    static void InitStructure(OMX_VIDEO_PARAM_PORTFORMATTYPE &structure );
    static void InitStructure(OMX_PARAM_PORTDEFINITIONTYPE &structure );

    static std::string IntToString( int a );

private:
    CommonFunctions();
};

#endif // COMMONFUNCTIONS_H
