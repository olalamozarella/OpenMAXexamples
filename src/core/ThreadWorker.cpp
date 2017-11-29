#include "ThreadWorker.h"

#include <pthread.h>

#include "src/core/Logger.h"

class ThreadWorker::DataClass
{
public:
    DataClass()
    {
        shouldFinish = false;
        pthread_mutex_init( &finishMutex, NULL );
    }

    ~DataClass()
    {
        pthread_mutex_destroy( &finishMutex );
    }

    pthread_t workerThread;
    bool shouldFinish;
    pthread_mutex_t finishMutex;
};

ThreadWorker::ThreadWorker()
{
    d = new DataClass();
}

ThreadWorker::~ThreadWorker()
{
    delete d;
}

bool ThreadWorker::Start()
{
    LOG_INFO( "ThreadWorker - Creating new thread" );
    pthread_create( &d->workerThread, NULL, DoSomething, this );
    return true;
}

bool ThreadWorker::Stop()
{
    LOG_INFO( "ThreadWorker - Stopping thread" );

    pthread_mutex_lock( &d->finishMutex );
    d->shouldFinish = true;
    pthread_mutex_unlock( &d->finishMutex );

    WaitForThreadJoin();

    return true;
}

bool ThreadWorker::WaitForThreadJoin()
{
    LOG_INFO( "ThreadWorker - Waiting for child thread join" );
    pthread_join( d->workerThread, NULL );
    LOG_INFO( "ThreadWorker - Thread joined" );
}

bool ThreadWorker::DoSomething()
{
    LOG_INFO( "Thread worker - call from other thread!" );
    return true;
}

void* ThreadWorker::DoSomething( void* context )
{
    ThreadWorker* instancePtr = static_cast<ThreadWorker*>( context );
    if ( instancePtr == NULL ) {
        LOG_ERR( "ThreadWorker - cannot run thread - thread instance is NULL" );
        return NULL;
    }

    bool ok = instancePtr->DoSomething();
    if ( ok == false ) {
        LOG_ERR( "ThreadWorker - thread finished with error" );
    } else {
        LOG_INFO( "ThreadWorker - thread finished successfully" );
    }

    return NULL;
}

bool ThreadWorker::ShouldFinish()
{
    bool result = false;

    pthread_mutex_lock( &d->finishMutex );
    result = d->shouldFinish;
    pthread_mutex_unlock( &d->finishMutex );

    return result;
}

