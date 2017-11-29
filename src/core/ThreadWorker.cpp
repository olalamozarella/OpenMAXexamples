#include "ThreadWorker.h"

#include "src/core/Logger.h"

ThreadWorker::ThreadWorker()
{
}

ThreadWorker::~ThreadWorker()
{
}

bool ThreadWorker::Setup()
{
    return true;
}

bool ThreadWorker::Start()
{
    LOG_INFO( "ThreadWorker - Creating new thread" );
    pthread_create( &workerThread, NULL, DoSomething, this );
    return true;
}

bool ThreadWorker::Stop()
{
    LOG_INFO( "ThreadWorker - Stopping thread" );
    pthread_join( workerThread, NULL );
    LOG_INFO( "ThreadWorker - Thread stopped" );
    return true;
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

