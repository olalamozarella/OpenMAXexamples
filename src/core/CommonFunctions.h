#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <string>

#include "IL/OMX_Core.h"

#define INT2STR CommonFunctions::IntToString
#define FLOAT2STR CommonFunctions::FloatToString

#define ASSERT(condition, errorMessage) \
    if ( condition ) { \
        LOG_ERR( errorMessage ); \
        return; \
    }

struct OMX_PARAM_PORTDEFINITIONTYPE;
struct OMX_CONFIG_REQUESTCALLBACKTYPE;
struct OMX_VIDEO_PARAM_PORTFORMATTYPE;
struct OMX_IMAGE_PARAM_PORTFORMATTYPE;
struct OMX_PARAM_U32TYPE;
struct OMX_CONFIG_REQUESTCALLBACKTYPE;
struct OMX_CONFIG_PORTBOOLEANTYPE;
struct OMX_VIDEO_PARAM_BITRATETYPE;
struct OMX_CONFIG_FRAMERATETYPE;

class CommonFunctions
{
public:
    static std::string ErrorToString( int errorID );
    static std::string StateToString( OMX_STATETYPE state );
    static std::string EventTypeToString( OMX_EVENTTYPE eventType , OMX_U32 data1 = 0 );

    static std::string GetComponentState( OMX_HANDLETYPE handle );
    static bool DisableAllPorts( OMX_HANDLETYPE handle );

    static void InitStructure( OMX_PORT_PARAM_TYPE& structure );
    static void InitStructure( OMX_PARAM_PORTDEFINITIONTYPE& structure );
    static void InitStructure( OMX_VIDEO_PARAM_PORTFORMATTYPE& structure );
    static void InitStructure( OMX_IMAGE_PARAM_PORTFORMATTYPE& structure );
    static void InitStructure( OMX_CONFIG_REQUESTCALLBACKTYPE& structure );
    static void InitStructure( OMX_PARAM_U32TYPE& structure );
    static void InitStructure( OMX_CONFIG_PORTBOOLEANTYPE& structure );
    static void InitStructure( OMX_VIDEO_PARAM_BITRATETYPE& structure );
    static void InitStructure( OMX_CONFIG_FRAMERATETYPE& structure );

    static std::string IntToString( int a );
    static std::string FloatToString( double a );

    static bool ReadFileToBuffer( std::ifstream& inputFile, OMX_BUFFERHEADERTYPE* buffer, bool& foundEOF );
    static bool WriteBufferToFile( std::ofstream& outputFile, OMX_BUFFERHEADERTYPE* buffer );

private:
    CommonFunctions();
};

#endif // COMMONFUNCTIONS_H
