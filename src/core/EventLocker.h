#ifndef EVENTLOCKER_H
#define EVENTLOCKER_H

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
    bool WaitForEvent();
    bool BroadcastEvent();

private:
    class DataClass;
    DataClass* d;
};

#endif // EVENTLOCKER_H
