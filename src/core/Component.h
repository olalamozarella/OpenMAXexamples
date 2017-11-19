#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include "IL/OMX_Component.h"

class Component
{
public:
    Component( const std::string& name );
    virtual ~Component();

    OMX_HANDLETYPE GetHandle();

    std::string GetComponentState();

    bool Init();
    bool DeInit();

    bool GetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr );
    bool SetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr );

    bool GetState( OMX_STATETYPE& state );
    bool ChangeState( const OMX_STATETYPE state );

    bool EnablePort( const OMX_U32 port );
    bool DisablePort( const OMX_U32 port );

    bool EnablePortBuffers( const OMX_U32 port );
    bool DisablePortBuffers( const OMX_U32 port );

    bool UseBuffer( OMX_BUFFERHEADERTYPE* bufferHeader, OMX_U32 portIndex, OMX_U32 bufferSize, OMX_U8* buffer );

    // static callbacks, passed to component in Init()
    static OMX_ERRORTYPE EventHandlerCallback( OMX_IN OMX_HANDLETYPE componentHandle, OMX_IN OMX_PTR appData, OMX_IN OMX_EVENTTYPE event,
            OMX_IN OMX_U32 data1, OMX_IN OMX_U32 data2, OMX_IN OMX_PTR eventData );
    static OMX_ERRORTYPE FillBufferDoneCallback( OMX_IN OMX_HANDLETYPE componentHandle, OMX_IN OMX_PTR appData, OMX_IN OMX_BUFFERHEADERTYPE* bufferHeader );
    static OMX_ERRORTYPE EmptyBufferDoneCallback( OMX_IN OMX_HANDLETYPE componentHandle, OMX_IN OMX_PTR appData, OMX_IN OMX_BUFFERHEADERTYPE* bufferHeader );

    // instance callbacks, adds event to list
    virtual OMX_ERRORTYPE eventHandler( OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2, OMX_PTR eventData );
    virtual OMX_ERRORTYPE fillBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader );
    virtual OMX_ERRORTYPE emptyBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader );

    void WaitForEvent( OMX_EVENTTYPE eventType, OMX_U32 data1, OMX_U32 data2, int msTimeout );

protected:
    static void OnTimeout();

private:
    class DataClass;
    DataClass* d;
};

#endif // COMPONENT_H
