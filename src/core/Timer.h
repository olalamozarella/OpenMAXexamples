#ifndef TIMER_H
#define TIMER_H

/**
 * @brief pthread timer implementation - Creates a new pthread which goes to sleep for specified time. Calls callback function after wakeup.
 */
class Timer
{
public:
    Timer();
    ~Timer();

    /**
     * @brief Start - starts timer
     * @param ms - after how many milliseconds should callback function be called
     * @param callback - pointer to callback function
     * @param isSingleShot - not yet implemented
     */
    void Start( int ms, void (*callback)() , bool isSingleShot = true );
    void Stop();

private:
    class DataClass;
    DataClass* d;
};

#endif // TIMER_H
