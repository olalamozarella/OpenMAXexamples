#include "Timer.h"

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <sys/time.h>

using namespace std;

typedef void ( *CallbackFunction )();

struct ThreadData {
    int interval;
    CallbackFunction callback;
    bool isStarted;
    bool isRunning;
    bool stop;
};

class Timer::DataClass
{
public:
    pthread_t timerThread;
    ThreadData threadData;

    DataClass();
    ~DataClass();

    static pthread_mutex_t threadDataMutex;
    static pthread_mutex_t waitMutex;
    static pthread_cond_t waitCondition;
    static pthread_mutex_t threadRunningMutex;
    static pthread_cond_t threadRunningCondition;

    static void* SleepHelper( void* context );
};

pthread_mutex_t Timer::DataClass::threadDataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Timer::DataClass::waitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Timer::DataClass::waitCondition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t Timer::DataClass::threadRunningMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t Timer::DataClass::threadRunningCondition = PTHREAD_COND_INITIALIZER;

Timer::DataClass::DataClass()
{
    pthread_mutex_lock( &threadDataMutex );
    threadData.callback = NULL;
    threadData.interval = 0;
    threadData.isStarted = false;
    threadData.isRunning = false;
    threadData.stop = false;
    pthread_mutex_unlock( &DataClass::threadDataMutex );
}

Timer::DataClass::~DataClass()
{
}

void* Timer::DataClass::SleepHelper( void* context )
{
    int result = 0;
    pthread_mutex_lock( &threadDataMutex );

    ThreadData* data = static_cast<ThreadData*>( context );
    if ( data == NULL ) {
        data->isStarted = false;
        pthread_mutex_unlock( &threadDataMutex );
        pthread_exit( &result );
    }

    data->isRunning = true;
    pthread_cond_broadcast( &threadRunningCondition );

    int seconds = data->interval / 1000;
    long nanoseconds = ( data->interval % 1000 ) * 1000000;

    struct timespec timeToWait;
    struct timeval now;
    gettimeofday( &now, NULL );
    timeToWait.tv_sec = now.tv_sec + seconds;
    timeToWait.tv_nsec = now.tv_usec * 1000 + nanoseconds;

    //const clock_t begin_time = clock();
    pthread_mutex_lock( &waitMutex );
    pthread_mutex_unlock( &threadDataMutex );
    pthread_cond_timedwait( &waitCondition, &waitMutex, &timeToWait );
    pthread_mutex_unlock( &waitMutex );
    //std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    pthread_mutex_lock( &threadDataMutex );
    if ( data->stop == false )
    {
        data->callback();
    }

    data->isStarted = false;
    pthread_mutex_unlock( &threadDataMutex );
    pthread_exit( &result );
}

Timer::Timer()
{
    d = new DataClass();
}

Timer::~Timer()
{
    delete d;
}

void Timer::Start( int ms, void ( *callback )(), bool /*isSingleShot*/ )
{
    pthread_mutex_lock( &d->threadDataMutex );
    bool isThreadRunning = d->threadData.isStarted;
    pthread_mutex_unlock( &d->threadDataMutex );

    if ( isThreadRunning == true ) {
        Stop();
    }

    pthread_mutex_lock( &d->threadDataMutex );
    d->threadData.interval = ms;
    d->threadData.callback = callback;
    d->threadData.isStarted = true;
    d->threadData.isRunning = false;
    pthread_mutex_unlock( &d->threadDataMutex );

    pthread_create( &d->timerThread, NULL, &DataClass::SleepHelper, &d->threadData );
}

void Timer::Stop()
{
    pthread_mutex_lock( &d->threadDataMutex );

    if ( ( d->threadData.isStarted == true ) && ( d->threadData.isRunning == false ) )
    {
        pthread_mutex_unlock( &d->threadDataMutex );
        pthread_mutex_lock( &d->threadRunningMutex );
        pthread_cond_wait( &d->threadRunningCondition, &d->threadRunningMutex );
        pthread_mutex_unlock( &d->threadRunningMutex );
    }
    else
    {
        pthread_mutex_unlock( &d->threadDataMutex );
    }

    pthread_mutex_lock( &d->threadDataMutex );
    if ( d->threadData.isStarted == true ) {
        d->threadData.stop = true;
        pthread_cond_broadcast( &d->waitCondition );
        pthread_mutex_unlock( &d->threadDataMutex );
        void* result;
        pthread_join( d->timerThread, &result );
        d->threadData.stop = false;
    }
    else
    {
        pthread_mutex_unlock( &d->threadDataMutex );
    }
}
