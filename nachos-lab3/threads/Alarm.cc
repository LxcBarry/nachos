#include "Alarm.h"
#include "system.h"
#include "copyright.h"
void check(int waiters)
{
    //防止当只有一个进程时系统终止
    // currentThread->Yield();
	while (waiters!=0)
	{
		currentThread->Yield();
	}
	currentThread->Finish();
}

static void AlarmHandler(int arg) 
{
    Alarm *p = (Alarm *)arg; 
    p->Call(); 
} 
Alarm::Alarm()
{
    queue = new List();
    waiters = 0;
    
}

Alarm::~Alarm()
{
    delete queue;
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
        // 当队列为空时，结束进程，队列只有一个进程时,防止系统终止
        Thread *loop_thread= new Thread("loop thread");
        loop_thread->Fork(check,waiters);
    }
    //TimerTicks=100,这个是howlong的单位，定义在machine/stat.h
    int Ticks = TimerTicks * how_long;
    //把wake up时刻插入
    queue->SortedInsert((void*)currentThread,stats->totalTicks  +Ticks);//按时序有序插入
    // char msg[60];
    sprintf(msg,"Alarm: now time is %d thread %s will wake up when time is %d \n",stats->totalTicks,
                                                                                    currentThread->getName(),
                                                                                    stats->totalTicks+Ticks);
    DEBUG('w', msg);

    //指定多少时刻后发生中断
    interrupt->Schedule(AlarmHandler,(int)this,Ticks,TimerInt);
    currentThread->Sleep();
    (void)interrupt->SetLevel(oldLevel);
}

void Alarm::Call()
{
    //中断时调用的函数
    //查看当前已经到达wakeup时间的进程，将它调入就绪队列
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
            queue->SortedInsert((void*)t,when);
            break;
        }

    }
    (void) interrupt->SetLevel(oldLevel); 
}