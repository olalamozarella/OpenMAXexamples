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

    OMX_ERRORTYPE DisableAllPorts();
    EventLocker* GetPortBufferLocker( const OMX_U32 port );

    Component* dataclassOwner;

    OMX_HANDLETYPE componentHandle;
    OMX_STRING componentName;

    list<Event> eventList;
    list<EmptyBufferDone> emptyBufferDoneList;
    list<FillBufferDone> fillBufferDoneList;

    map<OMX_U32, list<OMX_BUFFERHEADERTYPE*> > emptyBuffers;

    EventLocker eventLocker;
    map<OMX_U32, EventLocker*> portBufferLockers;
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
    LOG_INFO( dataclassOwner->GetComponentName() + ":Trying to disable all ports" );

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
            LOG_INFO( dataclassOwner->GetComponentName() + ":Disabling port " + INT2STR( portNumber ) );
            OMX_SendCommand( componentHandle, OMX_CommandPortDisable, portNumber, NULL );
            dataclassOwner->WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortDisable, portNumber, EVENT_HANDLER_TIMEOUT_MS_DEFAULT );
        }
    }

    LOG_INFO( dataclassOwner->GetComponentName() + ":All ports disabled" );
    return err;
}

EventLocker* Component::DataClass::GetPortBufferLocker( const OMX_U32 port )
{
    EventLocker* portBufferLocker = NULL;
    if ( portBufferLockers.find( port ) == portBufferLockers.end() ) {
        portBufferLocker = new EventLocker();
        portBufferLockers[ port ] = portBufferLocker;
    } else {
        portBufferLocker = portBufferLockers[port];
    }

    return portBufferLocker;
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

string Component::GetComponentName()
{
    return d->componentName;
}

bool Component::Init()
{
    OMX_CALLBACKTYPE callbacks;
    callbacks.EmptyBufferDone = EmptyBufferDoneCallback;
    callbacks.FillBufferDone = FillBufferDoneCallback;
    callbacks.EventHandler = EventHandlerCallback;

    OMX_ERRORTYPE err = OMX_GetHandle( &d->componentHandle, d->componentName, this, &callbacks );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_GetHandle failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    err = d->DisableAllPorts();
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":DisableAllPorts failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::DeInit()
{
    bool ok = false;
    for ( map<OMX_U32, EventLocker*>::iterator iter = d->portBufferLockers.begin(); iter != d->portBufferLockers.end(); iter++ ) {
        ok = iter->second->Deinit();
        if ( ok == false ) {
            LOG_ERR( GetComponentName() + ":Error deinit port buffer locker for port " + INT2STR( iter->first ) );
        }
        delete iter->second;
    }
    d->portBufferLockers.clear();

    ok = d->eventLocker.Deinit();
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error deinit eventLocker" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_FreeHandle( d->componentHandle );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_FreeHandle failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::GetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr )
{
    OMX_ERRORTYPE err = OMX_GetParameter( d->componentHandle, paramType, paramPtr );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_GetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::SetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr )
{
    OMX_ERRORTYPE err = OMX_SetParameter( d->componentHandle, paramType, paramPtr );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_SetParameter failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::GetState( OMX_STATETYPE& state )
{
    OMX_ERRORTYPE err = OMX_GetState( d->componentHandle, &state );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":OMX_GetState failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::ChangeState( const OMX_STATETYPE state )
{
    LOG_INFO( GetComponentName() + ":Change state - from " + CommonFunctions::GetComponentState( d->componentHandle ) + " to " + CommonFunctions::StateToString( state ) );

    OMX_ERRORTYPE err = OMX_SendCommand( d->componentHandle, OMX_CommandStateSet, state, NULL );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":Change state failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    WaitForEvent( OMX_EventCmdComplete, OMX_CommandStateSet, state, EVENT_HANDLER_TIMEOUT_MS_MAX );

    return true;
}

bool Component::EnablePort( const OMX_U32 port )
{
    LOG_INFO( GetComponentName() + ":Enabling port " + INT2STR( port ) + " on " + d->componentName );

    OMX_ERRORTYPE err = OMX_SendCommand( d->componentHandle, OMX_CommandPortEnable, port, NULL );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":Enable port failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::DisablePort( const OMX_U32 port )
{
    LOG_INFO( GetComponentName() + ":Disabling port " + INT2STR( port ) + " on " + d->componentName );

    OMX_ERRORTYPE err = OMX_SendCommand( d->componentHandle, OMX_CommandPortDisable, port, NULL );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":Disable port failed: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::EnablePortBuffers( const OMX_U32 port )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );

    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    CommonFunctions::InitStructure( portdef );
    portdef.nPortIndex = port;
    bool ok = GetParameter( OMX_IndexParamPortDefinition, &portdef );
    if ( ok == false ) {
        return false;
    }

    if ( ( portdef.bEnabled == OMX_TRUE ) || ( portdef.nBufferCountActual == 0 ) || ( portdef.nBufferSize == 0 ) ) {
        LOG_ERR( GetComponentName() + ":Cannot enable port buffers - port is not ready" );
        return false;
    }

    OMX_STATETYPE actualState;
    ok = GetState( actualState );
    if ( ok == false ) {
        return false;
    }
    if ( actualState != OMX_StateIdle ) {
        LOG_ERR( GetComponentName() + ":Cannot enable port buffers - wrong component state" );
        return false;
    }

    portBufferLocker->Lock();

    // port will be enabled after all buffers have been allocated
    ok = EnablePort( port );
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":Error enabling port" );
        portBufferLocker->Unlock();
        return false;
    }

    LOG_INFO( "Creating " + INT2STR( portdef.nBufferCountActual ) + " buffers" );
    for ( OMX_U32 bufferID = 0; bufferID < portdef.nBufferCountActual; bufferID++ ) {
        unsigned char* buffer;
        int result = posix_memalign( ( void** )&buffer, portdef.nBufferAlignment, portdef.nBufferSize );
        if ( result != 0 ) {
            LOG_ERR( GetComponentName() + ":Error allocating posix-aligned memory" );
            portBufferLocker->Unlock();
            return false;
        }

        OMX_BUFFERHEADERTYPE* bufferHeader;
        ok = UseBuffer( bufferHeader, port, portdef.nBufferSize, buffer );
        if ( ok == false ) {
            LOG_ERR( GetComponentName() + ":Error using allocated buffer" );
            portBufferLocker->Unlock();
            return false;
        }
    }

    if ( ( OMX_U32 )d->emptyBuffers[port].size() != portdef.nBufferCountActual ) {
        LOG_ERR( GetComponentName() + ":Allocated buffer count differs from port definition! Buffer count: " + INT2STR( d->emptyBuffers[port].size() ) + " Portdef: " + INT2STR( portdef.nBufferCountActual ) );
        portBufferLocker->Unlock();
        return false;
    }

    portBufferLocker->Unlock();

    WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortEnable, port, EVENT_HANDLER_TIMEOUT_MS_MAX );

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
        LOG_ERR( GetComponentName() + ":Cannot enable port buffers - port is not ready" );
        return false;
    }

    if ( ( OMX_U32 )d->emptyBuffers[port].size() != portdef.nBufferCountActual ) {
        LOG_ERR( GetComponentName() + ":Allocated buffer count differs from port definition! Buffer count: " + INT2STR( d->emptyBuffers[port].size() ) + " Portdef: " + INT2STR( portdef.nBufferCountActual ) );
        return false;
    }

    ok = DisablePort( port );
    if ( ok == false ) {
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*> buffers = d->emptyBuffers[port];
    for ( list<OMX_BUFFERHEADERTYPE*>::iterator iter = buffers.begin(); iter != buffers.end(); iter++  ) {
        OMX_ERRORTYPE err = OMX_FreeBuffer( d->componentHandle, port, ( *iter ) );
        if ( err != OMX_ErrorNone ) {
            LOG_ERR( GetComponentName() + ":Error during OMX_FreeBuffer(): " + CommonFunctions::ErrorToString( err ) );
            return false;
        }

        //free( ( *iter ) ); //je toto treba? dealokuje ich OMX_FreeBuffer?
    }
    buffers.clear();

    ok = WaitForEvent( OMX_EventCmdComplete, OMX_CommandPortDisable, port, 100 );
    if ( ok == false ) {
        LOG_ERR( "Disable port event has not arrived" );
        return false;
    }

    return true;
}

bool Component::GetBufferCount( const OMX_U32 port, int& actualCount, int& allocatedCount )
{
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    CommonFunctions::InitStructure( portdef );
    portdef.nPortIndex = port;
    bool ok = GetParameter( OMX_IndexParamPortDefinition, &portdef );
    if ( ok == false ) {
        return false;
    }

    allocatedCount = portdef.nBufferCountActual;
    actualCount = d->emptyBuffers[port].size();
    return true;
}

bool Component::UseBuffer( OMX_BUFFERHEADERTYPE* bufferHeader, OMX_U32 portIndex, OMX_U32 bufferSize, OMX_U8* buffer )
{
    //buffer header should be null - OMX_UseBuffer creates it
    OMX_ERRORTYPE err = OMX_UseBuffer( d->componentHandle, &bufferHeader, portIndex, NULL, bufferSize, buffer );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":Error in OMX_UseBuffer: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    d->emptyBuffers[portIndex].push_back( bufferHeader );

    return true;
}

bool Component::EmptyThisBuffer( OMX_BUFFERHEADERTYPE* buffer )
{
    if ( buffer == NULL ) {
        LOG_ERR( GetComponentName() + ":Null pointer" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_EmptyThisBuffer( d->componentHandle, buffer );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":Error during OMX_EmptyThisBuffer: " + CommonFunctions::ErrorToString( err ) );
        return false;
    }

    return true;
}

bool Component::FillThisBuffer( OMX_BUFFERHEADERTYPE* buffer )
{
    if ( buffer == NULL ) {
        LOG_ERR( GetComponentName() + ":Null pointer" );
        return false;
    }

    OMX_ERRORTYPE err = OMX_FillThisBuffer( d->componentHandle, buffer );
    if ( err != OMX_ErrorNone ) {
        LOG_ERR( GetComponentName() + ":Error during OMX_EmptyThisBuffer: " + CommonFunctions::ErrorToString( err ) );
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
                    LOG_INFO( GetComponentName() + ":Event handler - state set completed" );
                    break;
                case OMX_CommandPortDisable:
                    LOG_INFO( GetComponentName() + ":Event handler - port disable completed" );
                    break;
                case OMX_CommandPortEnable:
                    LOG_INFO( GetComponentName() + ":Event handler - port enable completed" );
                    break;
                case OMX_CommandFlush:
                    LOG_INFO( GetComponentName() + ":Event handler - flush completed" );
                    break;
                case OMX_CommandMarkBuffer:
                    LOG_INFO( GetComponentName() + ":Event handler - mark buffer completed" );
                    break;
            }
            break;
        case OMX_EventError:
            LOG_ERR( GetComponentName() + ":Event handler - error; data1: " + INT2STR( data1 ) + ", data2: " + INT2STR( data2 )  );
            break;
        case OMX_EventMark:
            LOG_INFO( GetComponentName() + ":Event handler - mark" );
            break;
        case OMX_EventPortSettingsChanged:
            LOG_WARN( GetComponentName() + ":Event handler - port settings changed" );
            break;
        case OMX_EventParamOrConfigChanged:
            LOG_WARN( GetComponentName() + ":Event handler - param or config changed" );
            break;
        case OMX_EventBufferFlag:
            LOG_INFO( GetComponentName() + ":Event handler - buffer flag" );
            break;
        case OMX_EventResourcesAcquired:
            LOG_INFO( GetComponentName() + ":Event handler - resources acquired" );
            break;
        case OMX_EventDynamicResourcesAvailable:
            LOG_INFO( GetComponentName() + ":Event handler - resources available" );
            break;
        default:
            LOG_WARN( GetComponentName() + ":Event handler - unknown event" );
            break;
    }

    Event event;
    event.eventType = eventType;
    event.data1 = data1;
    event.data2 = data2;
    event.eventData = eventData;

    bool ok = d->eventLocker.Lock();
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":EventHandler - cannot lock eventLocker" );
    }

    //LOG_WARN( "Storing event - data2: " + INT2STR(data2) );
    d->eventList.push_back( event );

    ok = d->eventLocker.Unlock();
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":EventHandler - cannot unlock eventLocker" );
    }

    ok = d->eventLocker.BroadcastEvent();
    if ( ok == false ) {
        LOG_ERR( GetComponentName() + ":EventHandler - cannot broadcast event" );
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Component::fillBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader )
{
    LOG_INFO( GetComponentName() + ":FillBufferDone event occured" );

    FillBufferDone event;
    event.buffer = bufferHeader;
    OMX_U32 port = bufferHeader->nOutputPortIndex;

    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();

    LOG_INFO( GetComponentName() + ":adding buffer to buffer list" );

    //buffer is filled - callback should just notify app
    //d->buffers[port].push_back( bufferHeader );

    LOG_INFO( GetComponentName() + ":buffer added to buffer list" );

    portBufferLocker->Unlock();

    //store event in list, so that other thread could access filled buffer
    d->fillBufferDoneList.push_back( event );

    //notify others that buffer is available
    portBufferLocker->BroadcastEvent();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Component::emptyBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader )
{
    LOG_INFO( GetComponentName() + ":EmptyBufferDone event occured" );

    EmptyBufferDone event;
    event.buffer = bufferHeader;
    OMX_U32 port = bufferHeader->nInputPortIndex;

    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();
    LOG_INFO( GetComponentName() + ":adding buffer to buffer list" );

    //add buffer header to buffer map
    d->emptyBuffers[port].push_back( bufferHeader );

    // I do not need to store emptyBufferDone events yet - nobody accesses those events for later use
    //d->emptyBufferDoneList.push_back( event );

    LOG_INFO( GetComponentName() + ":buffer added to buffer list" );
    portBufferLocker->Unlock();

    //notify others that buffer is available
    portBufferLocker->BroadcastEvent();

    return OMX_ErrorNone;
}

bool Component::WaitForEvent( const OMX_EVENTTYPE eventType, const OMX_U32 data1, const OMX_U32 data2, const int msTimeout )
{
    LOG_INFO( GetComponentName() + ":Waiting for event - " + CommonFunctions::EventTypeToString( eventType, data1 ) );

    bool foundMatch = false;
    while ( foundMatch == false ) {
        bool ok = d->eventLocker.Lock();
        if ( ok == false ) {
            LOG_ERR( GetComponentName() + ":Cannot lock eventLocker" );
            return false;
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
            LOG_ERR( GetComponentName() + ":Cannot unlock eventLocker" );
            return false;
        }

        if ( foundMatch == false ) {
            if ( msTimeout == EVENT_HANDLER_NO_TIMEOUT ) {
                return false; //do not wait for event, if it is not in current event list
            }

            ok = d->eventLocker.WaitForEvent( msTimeout );
            if ( ok == false ) {
                LOG_WARN( GetComponentName() + ": event did not come during timeout" + CommonFunctions::EventTypeToString( eventType, data1 ) );
                return false;
            }
        }
    }

    return true;
}

bool Component::WaitForBufferEvent( const OMX_U32 port, const int msTimeout )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    return portBufferLocker->WaitForEvent( msTimeout );
}

bool Component::RemoveBufferFromCollection( const OMX_U32 port, const OMX_BUFFERHEADERTYPE* buffer )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();
    if ( d->emptyBuffers.find( port ) == d->emptyBuffers.end() ) {
        LOG_ERR( GetComponentName() + ":Buffer list for this port does not exist" );
        portBufferLocker->Unlock();
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*>& bufferList = d->emptyBuffers[ port ];
    for ( list<OMX_BUFFERHEADERTYPE*>::iterator iter = bufferList.begin(); iter != bufferList.end(); iter++ ) {
        if ( *iter == buffer ) {
            LOG_INFO( GetComponentName() + ":Buffer erased from list" );
            bufferList.erase( iter );
            portBufferLocker->Unlock();
            return true;
        }
    }

    LOG_ERR( GetComponentName() + ":Cannot erase buffer from list - buffer is not in list" );
    portBufferLocker->Unlock();
    return false;
}

bool Component::WaitForInputBuffer( const OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();
    if ( d->emptyBuffers.find( port ) == d->emptyBuffers.end() ) {
        LOG_ERR( GetComponentName() + ":Buffer list for this port does not exist" );
        portBufferLocker->Unlock();
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*>& bufferList = d->emptyBuffers[ port ];
    if ( bufferList.size() == 0 ) {
        LOG_INFO( "No available buffer - waiting for available buffer" );
        portBufferLocker->Unlock();
        bool ok = portBufferLocker->WaitForEvent( EVENT_HANDLER_TIMEOUT_MS_MAX );
        if ( ok == false ) {
            LOG_ERR( "No available buffer after timeout passed" );
            return false;
        }
        portBufferLocker->Lock();
        LOG_INFO( GetComponentName() + ":Wakeup - new available buffer" );
    }

    buffer = bufferList.front();
    bufferList.pop_front();
    portBufferLocker->Unlock();
    return true;
}

bool Component::WaitForOutputBuffer( const OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();
    if ( d->emptyBuffers.find( port ) == d->emptyBuffers.end() ) {
        LOG_ERR( GetComponentName() + ":Buffer list for this port does not exist" );
        portBufferLocker->Unlock();
        return false;
    }

    if ( d->fillBufferDoneList.empty() == true ) {
        portBufferLocker->Unlock();
        bool ok = portBufferLocker->WaitForEvent( EVENT_HANDLER_TIMEOUT_MS_MAX );
        if ( ok == false ) {
            LOG_ERR( "No available buffer after timeout passed" );
            return false;
        }
        portBufferLocker->Lock();
        LOG_INFO( GetComponentName() + ":Wakeup - new available buffer" );
    }

    FillBufferDone event = d->fillBufferDoneList.front();
    buffer = event.buffer;
    if ( buffer == NULL ) {
        LOG_ERR( "Error waiting for output buffer - buffer in event is null" );
        return false;
    }

    d->fillBufferDoneList.pop_front();
    portBufferLocker->Unlock();
    return true;
}

bool Component::TakeOutputBufferFromCollection( const OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();
    if ( d->emptyBuffers.find( port ) == d->emptyBuffers.end() ) {
        LOG_ERR( GetComponentName() + ":Buffer list for this port does not exist" );
        portBufferLocker->Unlock();
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*>& bufferList = d->emptyBuffers[ port ];
    if ( bufferList.size() == 0 ) {
        LOG_ERR( "No buffers in collection" );
        return false;
    }

    buffer = bufferList.front();
    bufferList.pop_front();
    portBufferLocker->Unlock();
    return true;
}

bool Component::AddAllocatedBufferToMap( const OMX_U32 port, OMX_BUFFERHEADERTYPE* buffer )
{
    EventLocker* portBufferLocker = d->GetPortBufferLocker( port );
    portBufferLocker->Lock();

    if ( d->emptyBuffers.find( port ) == d->emptyBuffers.end() ) {
        LOG_ERR( GetComponentName() + ": Buffer list for this port does not exist" );
        portBufferLocker->Unlock();
        return false;
    }

    list<OMX_BUFFERHEADERTYPE*>& bufferList = d->emptyBuffers[ port ];
    bufferList.push_front( buffer );

    portBufferLocker->Unlock();

    return true;
}

string Component::GetComponentState()
{
    return CommonFunctions::GetComponentState( d->componentHandle );
}
