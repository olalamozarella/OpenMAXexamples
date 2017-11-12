#include "NoBuffers.h"

#include <iostream>
#include <unistd.h>
#include "IL/OMX_Component.h"
#include "src/CommonFunctions.h"

using namespace std;

OMX_STRING componentName = "OMX.broadcom.video_decode";

class NoBuffers::DataClass
{
public:
    DataClass()
    {
    }

    ~DataClass()
    {
    }

    static OMX_ERRORTYPE EventHandler( OMX_HANDLETYPE hComponent,
                                       OMX_PTR pAppData,
                                       OMX_EVENTTYPE eEvent,
                                       OMX_U32 Data1,
                                       OMX_U32 Data2,
                                       OMX_PTR pEventData );

};

OMX_ERRORTYPE NoBuffers::DataClass::EventHandler( OMX_HANDLETYPE /*hComponent*/, OMX_PTR /*pAppData*/, OMX_EVENTTYPE eEvent, OMX_U32 Data1, OMX_U32 Data2, OMX_PTR /*pEventData*/ )
{
    if ( eEvent == OMX_EventCmdComplete ) {
        if ( Data1 == OMX_CommandStateSet ) {
            cout << "Event handler: Component State changed to: ";
            switch ( ( int )Data2 ) {
                case OMX_StateInvalid:
                    cout << "OMX_StateInvalid" << endl;
                    break;
                case OMX_StateLoaded:
                    cout << "OMX_StateLoaded" << endl;
                    break;
                case OMX_StateIdle:
                    cout << "OMX_StateIdle" << endl;
                    break;
                case OMX_StateExecuting:
                    cout << "OMX_StateExecuting" << endl;
                    break;
                case OMX_StatePause:
                    cout << "OMX_StatePause" << endl;
                    break;
                case OMX_StateWaitForResources:
                    cout << "OMX_StateWaitForResources" << endl;
                    break;
            }
        } else  if ( Data1 == OMX_CommandPortEnable ) {
            cout << "Event handler: OMX State Port enabled " << ( int )Data2 << endl;
        } else if ( Data1 == OMX_CommandPortDisable ) {
            cout << "Event handler: OMX State Port disabled " << ( int )Data2 << endl;
        }
    } else if ( eEvent == OMX_EventBufferFlag ) {
        if ( ( int )Data2 == OMX_BUFFERFLAG_EOS ) {
            cout << "Event handler: Buffer end of stream" << endl;
        }
    } else if ( eEvent == OMX_EventError ) {
        if ( Data1 == OMX_ErrorSameState ) {
            cout << "Event handler: Error - Already in requested state" << endl;
        } else {
            cout << "Event handler: Error " << Data1 << endl;
        }
    } else  if ( eEvent == OMX_EventMark ) {
        cout << "Event handler: Buffer Mark" << endl;
    } else  if ( eEvent == OMX_EventPortSettingsChanged ) {
        cout << "Event handler: PortSettingsChanged" << endl;
    }

    return OMX_ErrorNone;
}

NoBuffers::NoBuffers()
    : TestCase( TESTCASE_NAME_NONWORKING )
{

}

NoBuffers::~NoBuffers()
{

}

void NoBuffers::Run()
{
    TestCase::Run();

    OMX_ERRORTYPE err = OMX_Init();
    if ( err != OMX_ErrorNone ) {
        cout << "OMX_Init failed" << CommonFunctions::ErrorToString( err ) << endl;
        return;
    }

    OMX_CALLBACKTYPE callbacks;
    callbacks.EventHandler = d->EventHandler;

    OMX_HANDLETYPE handle;
    err = OMX_GetHandle( &handle, componentName, NULL, &callbacks );
    if ( err != OMX_ErrorNone ) {
        cout << "OMX_GetHandle failed: " << CommonFunctions::ErrorToString( err ) << endl;
    }

    cout << "Actual state: " << CommonFunctions::GetComponentState( handle ) << endl;

    CommonFunctions::DisableAllPorts( handle );

    OMX_SendCommand( handle, OMX_CommandStateSet, OMX_StateIdle, NULL );
    cout << "Actual state: " << CommonFunctions::GetComponentState( handle ) << endl;

    OMX_SendCommand( handle, OMX_CommandStateSet, OMX_StateExecuting, NULL );
    cout << "Actual state: " << CommonFunctions::GetComponentState( handle ) << endl;

    err = OMX_Deinit();
    if ( err != OMX_ErrorNone ) {
        cout << "OMX_DeInit failed" << CommonFunctions::ErrorToString( err ) << endl;
        return;
    }
    cout << "OMX_Deinit successful" << endl;
}
