#include "EventLocker.h"

#include <pthread.h>
#include <sys/time.h>

#include "Logger.h"
#include "CommonFunctions.h"

#define DEFAULT_TIMEOUT_VALUE_MS      5

class EventLocker::DataClass
{
public:
    pthread_mutex_t mutex;
    pthread_mutex_t conditionMutex;
    pthread_cond_t condition;

    DataClass();
    ~DataClass();
};

EventLocker::DataClass::DataClass()
{
}

EventLocker::DataClass::~DataClass()
{
}

EventLocker::EventLocker()
{
    d = new DataClass();
}

EventLocker::~EventLocker()
{
    delete d;
}

bool EventLocker::Init()
{
    int result = pthread_mutex_init( &d->mutex, NULL );
    if ( result != 0 ) {
        LOG_ERR( "EventLocker - Mutex init failed: " + INT2STR( result ) );
        return false;
    }

    result = pthread_mutex_init( &d->conditionMutex, NULL );
    if ( result != 0 ){
        LOG_ERR( "EventLocker - conditionMutex init failed: " + INT2STR( result ) );
        pthread_mutex_destroy( &d->mutex );
        return false;
    }

    result = pthread_cond_init( &d->condition, NULL );
    if ( result != 0 ) {
        LOG_ERR( "EventLocker - condition init failed: " + INT2STR( result ) );
        pthread_mutex_destroy( &d->mutex );
        pthread_mutex_destroy( &d->conditionMutex );
        return false;
    }

    return true;
}

bool EventLocker::Deinit()
{
    int result = pthread_mutex_destroy( &d->mutex );
    result |= pthread_mutex_destroy( &d->conditionMutex );
    result |= pthread_cond_destroy( &d->condition );

    if ( result != 0 ) {
        LOG_ERR( "EventLocker - deinit failed: " + INT2STR( result ) );
        return false;
    }
}

bool EventLocker::Lock()
{
    struct timespec timeToWait;
    struct timeval now;
    gettimeofday( &now, NULL );
    timeToWait.tv_sec = now.tv_sec;
    timeToWait.tv_nsec = now.tv_usec + DEFAULT_TIMEOUT_VALUE_MS * 1000;

    int result = pthread_mutex_timedlock( &d->mutex, &timeToWait );
    if ( result != 0 ){
        LOG_ERR( "EventLocker - timedLock failed: " + INT2STR( result ) );
        return false;
    }

    return true;
}

bool EventLocker::Unlock()
{
    int result = pthread_mutex_unlock( &d->mutex );
    if ( result != 0 ){
        LOG_ERR( "EventLocker - unlock failed: " + INT2STR( result ) );
        return false;
    }

    return true;
}

bool EventLocker::WaitForEvent()
{
    struct timespec timeToWait;
    struct timeval now;
    gettimeofday( &now, NULL );
    timeToWait.tv_sec = now.tv_sec;
    timeToWait.tv_nsec = now.tv_usec + DEFAULT_TIMEOUT_VALUE_MS * 1000;

    int result = pthread_cond_timedwait( &d->condition, &d->conditionMutex, &timeToWait );
    if ( result != 0 ){
        LOG_ERR( "EventLocker - timedWait failed: " + INT2STR( result ) );
        return false;
    }

    return true;
}

bool EventLocker::BroadcastEvent()
{
    int result = pthread_cond_broadcast( &d->condition );
    if ( result != 0 ){
        LOG_ERR( "EventLocker - broadcast failed: " + INT2STR( result ) );
        return false;
    }
    return true;
}
