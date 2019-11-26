#include "Alarm.h"
#include "system.h"
#include "copyright.h"

static void AlarmHandler(int arg) 
{
     Alarm *p = (Alarm *)arg; 
    p->Call(); 
} 
Alarm::Alarm()
{
    queue = new List();
    waiters = 0;
    // loop_thread = new Thread("loop_thread");
    
}

Alarm::~Alarm()
{
    delete queue;
}
void check(int which)
{
    //防止当只有一个进程时系统终止
    // currentThread->Yield();
    
	while (which!=0)
	{
		currentThread->Yield();
	}
	currentThread->Finish();
}

void Alarm::Pause(int how_long)
{
    
    waiters++;
    char msg[80];
    // sprintf(msg,"%s will wake up after %d ticks\n",currentThread->getName(),how_long);
    // DEBUG('w',msg);
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    if(queue->IsEmpty()==TRUE)
    {
        // printf("*\n");
        // 用于防止系统终止
        Thread *loop_thread= new Thread("loop thread");
        loop_thread->Fork(check,waiters);
    }
    
    int Ticks = TimerTicks * how_long;
    // int Ticks = how_long;
    // queue->SortedInsert((void*)currentThread,stats->totalTicks+Ticks);//按时序有序插入
    queue->SortedInsert((void*)currentThread,stats->totalTicks  +Ticks);//按时序有序插入
    // char msg[60];
    sprintf(msg,"Alarm: now time is %d thread %s will wake up when time is %d \n",stats->totalTicks,
                                                                                    currentThread->getName(),
                                                                                    stats->totalTicks+Ticks);
    DEBUG('w', msg);
    interrupt->Schedule(AlarmHandler,(int)this,Ticks,TimerInt);
    currentThread->Sleep();
    (void)interrupt->SetLevel(oldLevel);
}

void Alarm::Call()
{
    int when;
    IntStatus oldLevel = interrupt->SetLevel(IntOff); 
    DEBUG('w',"calling...\n");
    Thread *t = (Thread *)queue->SortedRemove(&when);
    if(t==NULL) return;
    
    while(t != NULL)
    {
        //唤醒所有到时的进程
        if(when <= stats->totalTicks && t!= NULL)
        {
            waiters--;
            char msg[100];
            sprintf(msg,"Alarm:now time is :%d thread %s wakeup\n",stats->totalTicks,t->getName());
            DEBUG('w', msg);
            scheduler->ReadyToRun(t);
            t = (Thread*)queue->SortedRemove(&when);
        }
        else
        {
            // waiters++;
            queue->SortedInsert((void*)t,when);
            // interrupt->Schedule(AlarmHandler,(int)this,when,TimerInt); //指定时刻发生中断
            break;
        }

    }
    (void) interrupt->SetLevel(oldLevel); 
}