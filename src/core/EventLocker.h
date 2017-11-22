#ifndef EVENTLOCKER_H
#define EVENTLOCKER_H

#include "src/core/ProjectDefines.h"

/**
 * @brief Class for handling pthread_mutex_t and pthread_cond_t
 */
class EventLocker
{
public:
    EventLocker();
    ~EventLocker();

    bool Init();
    bool Deinit();

    bool Lock();
    bool Unlock();

    bool WaitForEvent( int msTimeout = EVENT_HANDLER_TIMEOUT_MS_DEFAULT );
    bool BroadcastEvent();

private:
    class DataClass;
    DataClass* d;
};

#endif // EVENTLOCKER_H
