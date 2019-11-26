#pragma once
#include "synch-sem.h"
class EventBarrier
{
private:
    int status;     //floor where currently stopped
    int waitnum;    //how many riders currently onboard
    Lock* lock1;
    Condition* signal;
    Lock* lock2;
    Condition* complete;
public:
    EventBarrier(/* args */);
    ~EventBarrier();
    void Wait();    //Wait until the event is signaled.Return immediately if already in the signaled state.
    void Signal();  //Signal the event and block until all threads that wait for this event have responded.
                    //The EventBarrier reverts to the unsignaled state when Signal() returns.
    void Complete();//Indicate that the calling thread has finished responding to a signaled event
                    //and block until all other threads that wait for this event have also responed.
    int Waiters();//Return a count of threads that are waiting for the event or taht have not yet responded to it.
};
