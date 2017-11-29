#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <pthread.h>

class ThreadWorker
{
public:
    ThreadWorker();
    ~ThreadWorker();

    virtual bool Setup();
    virtual bool Start();
    virtual bool Stop();
    virtual bool DoSomething();

    static void* DoSomething( void* context );

protected:
    pthread_t workerThread;
};

#endif // THREADWORKER_H
