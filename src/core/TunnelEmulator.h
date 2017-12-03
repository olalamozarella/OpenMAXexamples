#ifndef TUNNELEMULATOR_H
#define TUNNELEMULATOR_H

#include "IL/OMX_Core.h"

class Component;

/**
 * @brief The TunnelEmulator class - this class handles buffer handling between two components, emulating OMX tunnel.
 * Buffers are always allocated by producer component and used by consumer component. Buffer handling is done by separate thread pair.\
 *
 * One thread takes buffers from producer's FillBufferDone events and gives it to consumer in EmptyThisBuffer command, other thread
 * takes buffers from consumer's EmptyBufferDone event and gives it to producer in FillThisBuffer command.
 *
 * Producer sets EOS flag on last produced buffer, all allocated buffers are then returned to producer's buffer collection.
 */
class TunnelEmulator
{
public:
    TunnelEmulator( Component* producer, OMX_U32 producerPort, Component* consumer, OMX_U32 consumerPort );
    ~TunnelEmulator();

    /**
     * @brief SetupTunnel - allocates buffers in producer, calls UseBuffer on both producer and consumer
     * @return true if successful, false otherwise
     */
    bool SetupTunnel();

    /**
     * @brief StartTunnel - starts both threads, calls FillThisBuffer in producer for all buffers, removes buffers from producer's buffer collection
     * @return true if successful, false otherwise
     */
    bool StartTunnel();

    /**
     * @brief WaitForThreadJoin - waits for both threads finish
     * @return
     */
    bool WaitForThreadJoin();

    /**
     * @brief DestroyTunnel - deallocates all allocated buffers, removes all events
     * @return
     */
    bool DestroyTunnel();

private:
    class DataClass;
    DataClass* d;
};

#endif // TUNNELEMULATOR_H
