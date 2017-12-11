#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <list>

#include "IL/OMX_Component.h"

class Component
{
public:
    Component( const std::string& name );
    virtual ~Component();

    /**
     * @return Handle to the OMX_Component created in OMX_GetHandle()
     */
    OMX_HANDLETYPE GetHandle();

    /**
     * @return Component name from constructor
     */
    std::string GetComponentName();

    /**
     * @return Actual component state as string
     */
    std::string GetComponentState();

    /**
     * @brief Init - initializes callbacks, calls OMX_GetHandle, disables all ports
     * @return true if successful, false otherwise
     */
    bool Init();

    /**
     * @brief DeInit - clears event/port lockers, calls OMX_FreeHandle
     * @return true if successful, false otherwise
     */
    bool DeInit();

    /**
     * @brief GetParameter - OMX_GetParameter wrapper
     * @return true if successful, false otherwise
     */
    bool GetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr );

    /**
     * @brief SetParameter - OMX_SetParameter wrapper
     * @return true if successful, false otherwise
     */
    bool SetParameter( OMX_INDEXTYPE paramType, OMX_PTR paramPtr );

    /**
     * @brief GetState - OMX_GetStater wrapper
     * @return true if successful, false otherwise
     */
    bool GetState( OMX_STATETYPE& state );

    /**
     * @brief ChangeState - sends command to change state and waits for response
     * @param state - target state
     * @return true if successful, false otherwise
     */
    bool ChangeState( const OMX_STATETYPE state );

    /**
     * @brief EnablePort - sends command to enable port, does not wait for response
     * @return true if successful, false otherwise
     */
    bool EnablePort( const OMX_U32 port );

    /**
     * @brief DisablePort - sends command to disable port, does not wait for response
     * @return true if successful, false otherwise
     */
    bool DisablePort( const OMX_U32 port );

    /**
     * @brief EnablePortBuffers - allocates buffers and enables specified port, waits for PortEnabled event
     * @note buffer count is determined from OMX_PARAM_PORTDEFINITIONTYPE
     * @param port
     * @return true if successful, false otherwise
     */
    bool EnablePortBuffers( const OMX_U32 port );

    /**
     * @brief DisablePortBuffers - frees all buffers and disables specified port, waits for PortDisabled event
     * @param port
     * @return true if successful, false otherwise
     */
    bool DisablePortBuffers( const OMX_U32 port );

    /**
     * @brief GetBufferCount - returns actual and allocated buffer count for specified port
     * @return true if successful, false otherwise
     */
    bool GetBufferCount( const OMX_U32 port, int& actualCount, int& allocatedCount );

    /**
     * @brief UseBuffer - wrapper for OMX_UseBuffer + adds buffer to buffer map for specified port
     * @return true if successful, false otherwise
     */
    bool UseBuffer( OMX_BUFFERHEADERTYPE* bufferHeader, OMX_U32 portIndex, OMX_U32 bufferSize, OMX_U8* buffer );

    /**
     * @brief EmptyThisBuffer - wrapper for OMX_EmptyThisBuffer
     * @note Component should empty this buffer and return it through EmptyBufferDoneCallback
     * @return true if successful, false otherwise
     */
    bool EmptyThisBuffer( OMX_BUFFERHEADERTYPE* buffer );

    /**
     * @brief FillThisBuffer - wrapper for OMX_FillThisBuffer
     * @note Component should fill this buffer and return it through FillBufferDoneCallback
     * @return true if successful, false otherwise
     */
    bool FillThisBuffer( OMX_BUFFERHEADERTYPE* buffer );

    // Static callbacks, passed to component in Init(). Callbacks have to be static because they are passed to OMX_Component as function pointers
    // These callbacks are called when OMX_Component finishes working on them.
    static OMX_ERRORTYPE EventHandlerCallback( OMX_IN OMX_HANDLETYPE componentHandle, OMX_IN OMX_PTR appData, OMX_IN OMX_EVENTTYPE event,
            OMX_IN OMX_U32 data1, OMX_IN OMX_U32 data2, OMX_IN OMX_PTR eventData );
    static OMX_ERRORTYPE FillBufferDoneCallback( OMX_IN OMX_HANDLETYPE componentHandle, OMX_IN OMX_PTR appData, OMX_IN OMX_BUFFERHEADERTYPE* bufferHeader );
    static OMX_ERRORTYPE EmptyBufferDoneCallback( OMX_IN OMX_HANDLETYPE componentHandle, OMX_IN OMX_PTR appData, OMX_IN OMX_BUFFERHEADERTYPE* bufferHeader );

    // Instance callbacks. Event handling should be done here.
    virtual OMX_ERRORTYPE eventHandler( OMX_EVENTTYPE event, OMX_U32 data1, OMX_U32 data2, OMX_PTR eventData );
    virtual OMX_ERRORTYPE fillBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader );
    virtual OMX_ERRORTYPE emptyBufferDone( OMX_BUFFERHEADERTYPE* bufferHeader );

    /**
     * @brief WaitForEvent - waits until event with specified parameters occurs
     * @param msTimeout - maximal waiting time in milliseconds
     * @return true if successful, false otherwise
     */
    bool WaitForEvent( const OMX_EVENTTYPE eventType, const OMX_U32 data1, const OMX_U32 data2, const int msTimeout );

    /**
     * @brief WaitForBufferEvent - waits for buffer event ( Empty/FillBufferDone ) on specified port
     * @param port - port number
     * @param msTimeout - maximal waiting time in milliseconds
     * @return true if successful, false otherwise
     */
    bool WaitForBufferEvent( const OMX_U32 port, const int msTimeout ) ;

    /**
     * @brief RemoveBufferFromCollection - removes specified buffer from buffer list of specified port
     * @note Used in manual tunneling (sw tunnel emulation)
     * @return true if sucessful, false otherwise
     */
    bool RemoveBufferFromCollection( const OMX_U32 port, const OMX_BUFFERHEADERTYPE* buffer );

    /**
     * @brief GetInputBuffer - waits until free buffer header for specified port is available
     * @note waits for maximal possible time (@see ProjectDefines::EVENT_HANDLER_TIMEOUT_MS_MAX)
     * @param port
     * @param buffer
     * @return true if successful, false otherwise
     */
    bool WaitForInputBuffer( const OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer );

    /**
     * @brief WaitForOutputBuffer - waits until buffer event for specified port occurs, removes event from list, returns buffer
     * @param port
     * @param buffer
     * @return true if successful, false otherwise
     */
    bool WaitForOutputBuffer( const OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer );

    /**
     * @brief TakeOutputBufferFromCollection - takes buffer from port-buffer collection
     * @param port
     * @param buffer
     * @return
     */
    bool TakeOutputBufferFromCollection( const OMX_U32 port, OMX_BUFFERHEADERTYPE*& buffer );

    /**
     * @brief AddAllocatedBufferToMap - adds buffer header to buffer map for specified port
     * @param port
     * @param buffer
     * @return true if successful, false otherwise
     */
    bool AddAllocatedBufferToMap( const OMX_U32 port, OMX_BUFFERHEADERTYPE* buffer );

private:
    class DataClass;
    DataClass* d;
};

#endif // COMPONENT_H
