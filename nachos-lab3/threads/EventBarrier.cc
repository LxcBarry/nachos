#include "copyright.h"
#include "synch.h"
#include "system.h"
#include "EventBarrier.h"

EventBarrier::EventBarrier()
{
    waitnum=0;
    status=0;
    lock1=new Lock("lock1");
    signal=new Condition("signal");
    lock2=new Lock("lock2");
    complete= new Condition("complete");
}

EventBarrier::~EventBarrier()
{
}

void EventBarrier::Wait()
{
    waitnum++;
    if(status==1)
        return;
    else{
        lock1->Acquire();
        signal->Wait(lock1);
        lock1->Release();
    }
}

void EventBarrier::Signal()
{
    status=1;
    lock2->Acquire();
    lock1->Acquire();
    signal->Broadcast(lock1);
    lock1->Release();
    if(Waiters()!=0)
    {
        complete->Wait(lock2);
    }
    status=0;
    lock2->Release();
}

void EventBarrier::Complete()
{
    lock2->Acquire();
    waitnum--;
    if(Waiters()==0)
    {
        status=0;
        complete->Broadcast(lock2);
    }
    else{
        complete->Wait(lock2);
    }
    lock2->Release();
}

int EventBarrier::Waiters()
{
    return waitnum;
}
