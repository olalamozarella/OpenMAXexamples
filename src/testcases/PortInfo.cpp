#include "PortInfo.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "IL/OMX_Core.h"
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

    GetPortInfo( "OMX.broadcom.image_decode", 320 );
    GetPortInfo( "OMX.broadcom.image_decode", 321 );

    GetPortInfo( "OMX.broadcom.video_decode", 130 );
    GetPortInfo( "OMX.broadcom.video_decode", 131 );
}

void PortInfo::GetPortInfo( string componentName, int portNumber )
{
    char componentNameArray[128];
    strncpy( componentNameArray, componentName.c_str(), 128 );

    OMX_CALLBACKTYPE callbacks;
    callbacks.EmptyBufferDone = NULL;
    callbacks.EventHandler = NULL;
    callbacks.FillBufferDone = NULL;

    OMX_HANDLETYPE handle;
    OMX_ERRORTYPE err = OMX_GetHandle( &handle, componentNameArray, NULL, &callbacks );
    if ( err != OMX_ErrorNone ) {
        cout << "OMX GetHandle failed" << endl;
        return;
    }

    OMX_VERSIONTYPE specVersion, compVersion;
    OMX_UUIDTYPE uid;
    err = OMX_GetComponentVersion( handle, componentNameArray, &compVersion, &specVersion, &uid );
    if ( err != OMX_ErrorNone ) {
        cout << "OMX GetComponentVersion failed" << endl;
        return;
    }
    printf( "Component name: %s\nVersion %d.%d\nSpec version %d.%d\n", componentNameArray, compVersion.s.nVersionMajor, compVersion.s.nVersionMinor, specVersion.s.nVersionMajor, specVersion.s.nVersionMinor );

    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    portdef.nSize = sizeof( OMX_PARAM_PORTDEFINITIONTYPE );
    portdef.nVersion.nVersion = OMX_VERSION;
    portdef.nPortIndex = portNumber;

    if ( OMX_GetParameter( handle, OMX_IndexParamPortDefinition, &portdef ) != OMX_ErrorNone ) {
        cout << "OMX GetParameter failed" << endl;
        return;
    }

    PrintPortInfo( &portdef );
}

void PortInfo::PrintPortInfo( OMX_PARAM_PORTDEFINITIONTYPE* portdef )
{
    cout << "Port " << portdef->nPortIndex;

    if ( portdef->eDir ==  OMX_DirInput ) {
        cout << " is input port." << endl;
    } else {
        cout << " is output port." << endl;
    }

    string domain;
    switch ( portdef->eDomain ) {
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
    cout << " Buffer count: " << portdef->nBufferCountActual << endl;
    cout << " Buffer minimal count: " << portdef->nBufferCountMin << endl;
    cout << " Buffer size: " << portdef->nBufferSize << "bytes" << endl << endl;
}

