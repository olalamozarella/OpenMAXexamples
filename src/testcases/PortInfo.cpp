#include "PortInfo.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

//#include "IL/OMX_Core.h"
#include "IL/OMX_Component.h"

PortInfo::PortInfo()
    : TestCase( TESTCASE_NAME_PORT_INFO )
{
}

PortInfo::~PortInfo()
{
}

void PortInfo::Run()
{
    TestCase::Run();

    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        cout << "OMX_Init() failed" << endl;
        return;
    }

    OMX_HANDLETYPE handle;
    OMX_VERSIONTYPE specVersion;
    bool ok = GetComponentInfo( "OMX.broadcom.video_decode", &handle, &specVersion );
    if ( ok == false ) {
        return;
    }
    //PrintPortInfo( handle, 131 );
    PrintAllVideoPortsInfo( handle, specVersion );
}

bool PortInfo::GetComponentInfo( string componentName, OMX_HANDLETYPE* handle, OMX_VERSIONTYPE* specVersion )
{
    char componentNameArray[128];
    strncpy( componentNameArray, componentName.c_str(), 128 );

    OMX_CALLBACKTYPE callbacks;
    callbacks.EmptyBufferDone = NULL;
    callbacks.EventHandler = NULL;
    callbacks.FillBufferDone = NULL;

    OMX_ERRORTYPE err = OMX_GetHandle( handle, componentNameArray, NULL, &callbacks );
    if ( err != OMX_ErrorNone ) {
        cout << "OMX GetHandle failed" << endl;
        return false;
    }
    if ( handle == NULL ) {
        cout << "Null handle" << endl;
        return false;
    }

    OMX_VERSIONTYPE compVersion;
    OMX_UUIDTYPE uid;
    err = OMX_GetComponentVersion( *handle, componentNameArray, &compVersion, specVersion, &uid );
    if ( err != OMX_ErrorNone ) {
        cout << "OMX GetComponentVersion failed" << endl;
        return false;
    }
    printf( "Component name: %s\nVersion %d.%d\nSpec version %d.%d\n", componentNameArray, compVersion.s.nVersionMajor, compVersion.s.nVersionMinor, specVersion->s.nVersionMajor, specVersion->s.nVersionMinor );

    return true;
}

void PortInfo::PrintPortInfo( OMX_HANDLETYPE& handle, int portNumber )
{
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    portdef.nSize = sizeof( OMX_PARAM_PORTDEFINITIONTYPE );
    portdef.nVersion.nVersion = OMX_VERSION;
    portdef.nPortIndex = portNumber;

    if ( OMX_GetParameter( handle, OMX_IndexParamPortDefinition, &portdef ) != OMX_ErrorNone ) {
        cout << "OMX GetParameter failed" << endl;
        return;
    }

    cout << "Port " << portdef.nPortIndex;

    if ( portdef.eDir ==  OMX_DirInput ) {
        cout << " is input port." << endl;
    } else {
        cout << " is output port." << endl;
    }

    string domain;
    switch ( portdef.eDomain ) {
        case OMX_PortDomainAudio:
            domain = "Audio";
            break;
        case OMX_PortDomainVideo:
            domain = "Video";
            break;
        case OMX_PortDomainImage:
            domain = "Image";
            break;
        case OMX_PortDomainOther:
            domain = "Other";
            break;
    }
    cout << " Domain is: " << domain << endl;
    cout << " Buffer count: " << portdef.nBufferCountActual << endl;
    cout << " Buffer minimal count: " << portdef.nBufferCountMin << endl;
    cout << " Buffer size: " << portdef.nBufferSize << "bytes" << endl << endl;
}

void PortInfo::PrintAllVideoPortsInfo( OMX_HANDLETYPE& handle, OMX_VERSIONTYPE& specVersion )
{
    OMX_PORT_PARAM_TYPE param;
    param.nVersion.s.nVersionMajor = specVersion.s.nVersionMajor;
    param.nVersion.s.nVersionMinor = specVersion.s.nVersionMinor;
    param.nVersion.s.nRevision = specVersion.s.nRevision;
    param.nVersion.s.nStep = specVersion.s.nStep;
    param.nSize = sizeof( OMX_PORT_PARAM_TYPE );

    OMX_ERRORTYPE err = OMX_GetParameter( handle, OMX_IndexParamVideoInit, &param );
    if ( err != OMX_ErrorNone ) {
        cout << "Error in getting video OMX_PORT_PARAM_TYPE parameter: " << hex << ( int )err << endl;
        return;
    }

    cout << "Ports start on " << param.nStartPortNumber << endl;
    cout << "There are " << param.nPorts <<  " open ports" << endl;
}
