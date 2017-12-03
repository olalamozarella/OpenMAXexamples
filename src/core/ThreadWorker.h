#ifndef THREADWORKER_H
#define THREADWORKER_H

/**
 * @brief This class is a class wrapper for pthread_t thread.
 * @note Children of this class should always override DoSomething() method.
 */
class ThreadWorker
{
public:
    ThreadWorker();
    virtual ~ThreadWorker();

    /**
     * @brief Start - creates a new thread, which starts executing DoSomething() method.
     * @note Think twice before overriding this method in child classes.
     * @return true if successful, false otherwise
     */
    virtual bool Start();

    /**
     * @brief Stop - blocking wait until child thread finishes
     * @note Override this method, if other behaviour is desirable (forced exit of thread, ... ).
     * @return true if successful, false otherwise
     */
    virtual bool Stop();

    /**
     * @brief WaitForThreadJoin - blocks calling thread until child thread exits
     * @return
     */
    virtual bool WaitForThreadJoin();

    /**
     * @brief DoSomething - this method is executed by new thread
     * @note This method should be always implemented in child classes.
     * @note Method should regularly check for ShouldFinish flag and terminate as soon as possible when this flag is set.
     * @return true if successful, false otherwise
     */
    virtual bool DoSomething();

    /**
     * @brief DoSomething - helper function for executing threads
     * @param context - pointer to ThreadWorker instance casted to void*
     * @return NULL
     */
    static void* DoSomething( void* context );

protected:
    /**
     * @brief ShouldFinish - returns shouldFinish flag
     * @note Thread should exit as soon as possible when this flag is set.
     */
    bool ShouldFinish();

private:
    class DataClass;
    DataClass* d;
};

#endif // THREADWORKER_H
