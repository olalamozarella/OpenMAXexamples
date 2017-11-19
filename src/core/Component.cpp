#include "Component.h"

#include <iostream>
#include <cstring>
#include <list>
#include <map>
#include <cstdlib>
#include <unistd.h>

#include "Callbacks.h"
#include "CommonFunctions.h"
#include "Logger.h"
#include "Timer.h"
#include "EventLocker.h"

using namespace std;

class Component::DataClass
{
public:
    DataClass( Component* owner );
    ~DataClass();

    Component* dataclassOwner;

    OMX_ERRORTYPE DisableAllPorts();

    OMX_HANDLETYPE componentHandle;
    OMX_STRING componentName;

    list<Event> eventList;
    list<EmptyBufferDone> emptyBufferDoneList;
    list<FillBufferDone> fillBufferDoneList;

    map<OMX_U32, list<OMX_BUFFERHEADERTYPE*> > buffers;

    EventLocker eventLocker;
    EventLocker bufferLocker;
};

Component::DataClass::DataClass( Component* owner )
{
    dataclassOwner = owner;
    componentHandle = NULL;
    componentName = NULL;
}

Component::DataClass::~DataClass()
{
}

OMX_ERRORTYPE Component::DataClass::DisableAllPorts()
{
    OMX_ERRORTYPE err = OMX_ErrorNone;
    LOG_INFO( "Trying to disable all ports" );

    OMX_INDEXTYPE types[] = { OMX_IndexParamAudioInit, OMX_IndexParamVideoInit, OMX_IndexParamImageInit, OMX_IndexParamOtherInit };
    for ( int i = 0; i < 4; i++ ) {
        OMX_PORT_PARAM_TYPE param;
        CommonFunctions::InitStructure( param );

        err = OMX_GetParameter( componentHandle, types[i], &param );
        if ( err != OMX_ErrorNone ) {
            return err;
        }

        int firstPort = param.nStartPortNumber;
        int portCount = param.nPorts;
        for ( int portNumber = firstPort; portNumber < firstPort + portCount; portNumber++ ) {
            LOG_INFO( "Disabling port " + INT2STR( portNumber ) );
            OMX_SendCommand( componentHandle, OMX_CommandPortDisable, portNumber, NULL );
            dataclassOwner->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortDisable, portNumber, 0 );
        }
    }

    LOG_INFO( "All ports disabled" );
    return err;
}

Component::Component( const string& name )
{
    d = new DataClass( this );

    d->componentName = new char[ name.size() + 1 ];
    copy( name.begin(), name.end(), d->componentName );
    d->componentName[ name.size() ] = '\0';
}

Component::~Component()
{
    delete[] d->componentName;
    delete d;
}

OMX_HANDLETYPE Component::GetHandle()
{
    return d->componentHandle;
}

bool Component::Init()
{
    bool ok = d->eventLocker.Init();
    if ( ok == false ) {
        LOG_ERR( "Error init eventLocker" );
        return false;
    }

    ok = d->bufferLocker.Init();
    if ( ok == false ) {
        LOG_ERR( "Error init bufferLocker" );
        return false;
    }

    OMX_CALLBACKTYPE callbacks;
    callbacks.EmptyBufferDone = EmptyBufferDoneCallback;
    callbacks.FillBufferDone = FillBufferDoneCallback;
    callbacks.EventHandler = EventHandlerCallback;

    OMX_ERRORTYPE err = OMX_GetHandle( &d->componentHandle, d->componentName, this, &callbacks );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_GetHandle failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    err = d->DisableAllPorts();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "DisableAllPorts failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::DeInit()
{
    bool ok = d->eventLocker.Deinit();
    if ( ok == false ) {
        LOG_ERR( "Error deinit eventLocker" );
        return false;
    }

    ok = d->bufferLocker.Deinit();
    if ( ok == false ) {
        LOG_ERR( "Error deinit bufferLocker" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_FreeHandle( d->componentHandle );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_FreeHandle failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::GetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr )
{
    OMX_ERRORTYPE err = OMX_GetParameter( d->componentHandle, paramType, paramPtr );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_GetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::SetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr )
{
    OMX_ERRORTYPE err = OMX_SetParameter( d->componentHandle, paramType, paramPtr );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::GetState( OMX_STATETYPE& state )
{
    OMX_ERRORTYPE err = OMX_GetState( d->componentHandle, &state );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "OMX_GetState failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::ChangeState( const OMX_STATETYPE state )
{
    LOG_INFO( "Change state - from " + CommonFunctions::GetComponentState( d->componentHandle ) + " to " + CommonFunctions::StateToString( state ) );

    OMX_ERRORTYPE err = OMX_SendCommand( d->componentHandle, OMX_CommandStateSet, state, NULL );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "Change state failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    WaitForEvent( OMX_EventCmdComplete, OMX_CommandStateSet, state, 0 );

    return true;
}

bool Component::EnablePort( const OMX_U32 port )
{
    LOG_INFO( "Enabling port " + INT2STR( port ) + " on " + d->componentName );

    OMX_ERRORTYPE err = OMX_SendCommand( d->componentHandle, OMX_CommandPortEnable, port, NULL );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "Enable port failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::DisablePort( const OMX_U32 port )
{
    LOG_INFO( "Disabling port " + INT2STR( port ) + " on " + d->componentName );

    OMX_ERRORTYPE err = OMX_SendCommand( d->componentHandle, OMX_CommandPortDisable, port, NULL );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "Disable port failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::EnablePortBuffers( const OMX_U32 port )
{
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    CommonFunctions::InitStructure( portdef );
    portdef.nPortIndex = port;
    bool ok = GetParameter( OMX_IndexParamPortDefinition, &portdef );
    if ( ok == false ) {
        return false;
    }

    if ( ( portdef.bEnabled == OMX_TRUE ) || ( portdef.nBufferCountActual == 0 ) || ( portdef.nBufferSize == 0 ) ) {
        LOG_ERR( "Cannot enable port buffers - port is not ready" );
        return false;
    }

    OMX_STATETYPE actualState;
    ok = GetState( actualState );
    if ( ok == false ) {
        return false;
    }
    if ( actualState != OMX_StateIdle ) {
        LOG_ERR( "Cannot enable port buffers - wrong component state" );
        return false;
    }

    d->bufferLocker.Lock();

    // port will be enabled after all buffers have been allocated
    ok = EnablePort( port );
    if ( ok == false ) {
        LOG_ERR( "Error enabling port" );
        d->bufferLocker.Unlock();
        return false;
    }

    LOG_INFO( "Creating " + INT2STR( portdef.nBufferCountActual ) + " buffers" );
    for ( OMX_U32 bufferID = 0; bufferID < portdef.nBufferCountActual; bufferID++ ) {
        unsigned char* buffer;
        int result = posix_memalign( ( void** )&buffer, portdef.nBufferAlignment, portdef.nBufferSize );
        if ( result != 0 ) {
            LOG_ERR( "Error allocating posix-aligned memory" );
            d->bufferLocker.Unlock();
            return false;
        }

        OMX_BUFFERHEADERTYPE* bufferHeader;
        ok = UseBuffer( bufferHeader, port, portdef.nBufferSize, buffer );
        if ( ok == false ) {
            LOG_ERR( "Error using allocated buffer" );
            d->bufferLocker.Unlock();
            return false;
        }
    }

    if ( ( OMX_U32 )d->buffers[port].size() != portdef.nBufferCountActual ) {
        LOG_ERR( "Allocated buffer count differs from port definition! Buffer count: " + INT2STR( d->buffers[port].size() ) + " Portdef: " + INT2STR( portdef.nBufferCountActual ) );
        d->bufferLocker.Unlock();
        return false;
    }

    d->bufferLocker.Unlock();

    WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, port, 0 );

    return true;
}

bool Component::DisablePortBuffers( const OMX_U32 port )
{
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    CommonFunctions::InitStructure( portdef );
    portdef.nPortIndex = port;
    bool ok = GetParameter( OMX_IndexParamPortDefinition, &portdef );
    if ( ok == false ) {
        return false;
    }

    if ( ( portdef.bEnabled == OMX_FALSE ) || ( portdef.nBufferCountActual == 0 ) || ( portdef.nBufferSize == 0 ) ) {
        LOG_ERR( "Cannot enable port buffers - port is not ready" );
        return false;
    }

    if ( ( OMX_U32 )d->buffers[port].size() != portdef.nBufferCountActual ) {
        LOG_ERR( "Allocated buffer count differs from port definition! Buffer count: " + INT2STR( d->buffers[port].size() ) + " Portdef: " + INT2STR( portdef.nBufferCountActual ) );
        return false;
    }

    ok = DisablePort( port );
    if ( ok == false ) {
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*> buffers = d->buffers[port];
    for ( list<OMX_BUFFERHEADERTYPE*>::iterator iter = buffers.begin(); iter != buffers.end(); iter++  ) {
        OMX_ERRORTYPE err = OMX_FreeBuffer( d->componentHandle, port, ( *iter ) );
        if ( err != OMX_ErrorNone ) {
            LOG_ERR( "Error during OMX_FreeBuffer(): " + CommonFunctions::ErrorToString( err ) );
            return false;
        }
    }
    buffers.clear();

    WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortDisable, port, 0 );

    return true;
}

bool Component::UseBuffer( OMX_BUFFERHEADERTYPE* bufferHeader, OMX_U32 portIndex, OMX_U32 bufferSize, OMX_U8* buffer )
{
    if ( bufferHeader == NULL ) {
        LOG_ERR( "Null pointer" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_UseBuffer( d->componentHandle, &bufferHeader, portIndex, NULL, bufferSize, buffer );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "Error in OMX_UseBuffer: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    d->buffers[portIndex].push_back( bufferHeader );

    return true;
}

bool Component::EmptyThisBuffer( OMX_BUFFERHEADERTYPE* buffer )
{
    if ( buffer == NULL ) {
        LOG_ERR( "Null pointer" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_EmptyThisBuffer( d->componentHandle, buffer );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( "Error during OMX_EmptyThisBuffer: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

OMX_ERRORTYPE Component::EventHandlerCallback( OMX_HANDLETYPE /*componentHandle*/, OMX_PTR appData, OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2, OMX_PTR eventData )
{
    Component* componentInstance = ( Component* )appData;
    if ( componentInstance == NULL ) {
        LOG_ERR( "Event handler callback - appdata is not component" );
        return OMX_ErrorComponentNotFound;
    }

    return componentInstance->eventHandler( event, data1, data2, eventData );
}

OMX_ERRORTYPE Component::FillBufferDoneCallback( OMX_HANDLETYPE /*componentHandle*/, OMX_PTR appData, OMX_BUFFERHEADERTYPE* bufferHeader )
{
    Component* componentInstance = ( Component* )appData;
    if ( componentInstance == NULL ) {
        LOG_ERR( "Fill buffer done callback - appdata is not component" );
        return OMX_ErrorComponentNotFound;
    }

    return componentInstance->fillBufferDone( bufferHeader );
}

OMX_ERRORTYPE Component::EmptyBufferDoneCallback( OMX_HANDLETYPE /*componentHandle*/, OMX_PTR appData, OMX_BUFFERHEADERTYPE* bufferHeader )
{
    Component* componentInstance = ( Component* )appData;
    if ( componentInstance == NULL ) {
        LOG_ERR( "Empty buffer done callback - appdata is not component" );
        return OMX_ErrorComponentNotFound;
    }

    return componentInstance->emptyBufferDone( bufferHeader );
}

OMX_ERRORTYPE Component::eventHandler( OMX_EVENTTYPE eventType, OMX_U32 data1, OMX_U32 data2, OMX_PTR eventData )
{
    switch ( eventType ) {
        case OMX_EventCmdComplete:
            switch ( data1 ) {
                case OMX_CommandStateSet:
                    LOG_INFO( "Event handler - state set completed" );
                    break;
                case OMX_CommandPortDisable:
                    LOG_INFO( "Event handler - port disable completed" );
                    break;
                case OMX_CommandPortEnable:
                    LOG_INFO( "Event handler - port enable completed" );
                    break;
                case OMX_CommandFlush:
                    LOG_INFO( "Event handler - flush completed" );
                    break;
                case OMX_CommandMarkBuffer:
                    LOG_INFO( "Event handler - mark buffer completed" );
                    break;
            }
            break;
        case OMX_EventError:
            LOG_INFO( "Event handler - error" );
            break;
        case OMX_EventMark:
            LOG_INFO( "Event handler - mark" );
            break;
        case OMX_EventPortSettingsChanged:
            LOG_INFO( "Event handler - port settings changed" );
            break;
        case OMX_EventParamOrConfigChanged:
            LOG_INFO( "Event handler - param or config changed" );
            break;
        case OMX_EventBufferFlag:
            LOG_INFO( "Event handler - buffer flag" );
            break;
        case OMX_EventResourcesAcquired:
            LOG_INFO( "Event handler - resources acquired" );
            break;
        case OMX_EventDynamicResourcesAvailable:
            LOG_INFO( "Event handler - resources available" );
            break;
        default:
            LOG_WARN( "Event handler - unknown event" );
            break;
    }

    Event event;
    event.eventType = eventType;
    event.data1 = data1;
    event.data2 = data2;
    event.eventData = eventData;

    bool ok = d->eventLocker.Lock();
    if ( ok == false ) {
        LOG_ERR( "EventHandler - cannot lock eventLocker" );
    }

    //LOG_WARN( "Storing event - data2: " + INT2STR(data2) );
    d->eventList.push_back( event );

    ok = d->eventLocker.Unlock();
    if ( ok == false ) {
        LOG_ERR( "EventHandler - cannot unlock eventLocker" );
    }

    ok = d->eventLocker.BroadcastEvent();
    if ( ok == false ) {
        LOG_ERR( "EventHandler - cannot broadcast event" );
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Component::fillBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader )
{
    FillBufferDone event;
    event.buffer = bufferHeader;
    d->fillBufferDoneList.push_back( event );

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Component::emptyBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader )
{
    LOG_INFO( "EmptyBufferDone event occured" );

    EmptyBufferDone event;
    event.buffer = bufferHeader;
    OMX_U32 port = bufferHeader->nInputPortIndex;

    d->bufferLocker.Lock();
    LOG_INFO( "adding buffer to buffer list" );

    //add buffer header to buffer map
    d->buffers[port].push_back( bufferHeader );

    //add event to event list
    d->emptyBufferDoneList.push_back( event );

    LOG_INFO( "buffer added to buffer list" );
    d->bufferLocker.Unlock();

    //notify others that buffer is available
    d->bufferLocker.BroadcastEvent();

    return OMX_ErrorNone;
}

void Component::WaitForEvent( OMX_EVENTTYPE eventType, OMX_U32 data1, OMX_U32 data2, int msTimeout )
{
    LOG_INFO( "Waiting for event" );

    bool foundMatch = false;
    while ( foundMatch == false ) {
        bool ok = d->eventLocker.Lock();
        if ( ok == false ) {
            LOG_ERR( "Cannot lock eventLocker" );
            return;
        }

        for ( list<Event>::iterator iter = d->eventList.begin(); iter != d->eventList.end(); iter++ ) {
            if ( ( iter->eventType == eventType ) && ( iter->data1 == data1 ) && ( iter->data2 == data2 ) ) {
                foundMatch = true;
                d->eventList.erase( iter );
                break;
            }
        }

        ok = d->eventLocker.Unlock();
        if ( ok == false ) {
            LOG_ERR( "Cannot unlock eventLocker" );
            return;
        }

        if ( foundMatch == false ) {
            ok = d->eventLocker.WaitForEvent();
            if ( ok == false ) {
                LOG_ERR( "error waiting for event" );
                return;
            }
        }
    }
}

bool Component::GetInputBuffer( OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer )
{
    d->bufferLocker.Lock();
    if ( d->buffers.find( port ) == d->buffers.end() ) {
        LOG_ERR( "Buffer for this port does not exist" );
        d->bufferLocker.Unlock();
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*> bufferList = d->buffers[ port ];
    if ( bufferList.size() == 0 ) {
        LOG_INFO( "No available buffer - waiting for available buffer" );
        d->bufferLocker.Unlock();
        d->bufferLocker.WaitForEvent();
        d->bufferLocker.Lock();
        LOG_INFO( "Wakeup - new available buffer" );
    }

    buffer = bufferList.front();
    bufferList.pop_front();
    d->bufferLocker.Unlock();
    return true;
}

string Component::GetComponentState()
{
    return CommonFunctions::GetComponentState( d->componentHandle );
}
