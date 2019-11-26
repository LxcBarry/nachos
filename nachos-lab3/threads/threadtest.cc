// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "dllist.h"
#include "synch-sleep.h"
#include "Table.h"
#include "BoundedBuffer.h"
#include "stdio.h"
#include "EventBarrier.h"
#include "Alarm.h"
#include "Elevator.h"
// #include "synch-sem.h"

// testnum is set in main.cc
int testnum = 1;
int T=2;
int N=2;
int E=0;
DLList *	list;

// sleep lock
Lock * sleep_lock;

// table
#define TABLESIZE 10
Table * table;
Semaphore * t_buf;
Semaphore * t_msg;

// dllist(problem of produser and consumer)
// monitor
Condition * d_notFull;
Condition * d_notEmpty;
int count;
Lock * d_mutex;
// still use list
// still use N (as buffer max space)

// boundedBuffer
int b_in_size=2;
int b_out_size=2;
int b_maxsize=10;
BoundedBuffer * boundbuffer;
void InsertList(int N, DLList *list);
void RemoveList(int N, DLList *list);

//eventbarrier
EventBarrier *barr=new EventBarrier();

//Alarm
int alarm_T = 5;
int alarm_time_list[10]={1,2,3,4,5,0,0,0,0,0};
Alarm *alarm=new Alarm();

//Elevator and Building
Building *build = new Building("Building1",20,1);
int src[5] = {2,5,9,3,18};
int des[5] = {17,3,4,15,6};
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

void TestDllist(int which)
{
	//printf("*** thread %d ***\n",which);
	// DEBUG('s', "use lock...\n");
	//sleep_lock->Acquire();
	InsertList(N,list);
	if(E==1){
		//1号错误
		currentThread->Yield();
	}
	RemoveList(N,list);
	//sleep_lock->Release();
	//printf("*** thread %d ***\n",which);
	if(list->IsEmpty())
	{
		printf("*** thread %d :empty ***\n",which);
	}


	currentThread->Finish(); // give back the space

}


// TableProducer
void Table_producer(int which)
{

	while (true)
	{
		void *item;
		int key = Random() % 100;
		int *items = new int;
		items[0] = key;
		item = items;
		t_buf->P();
		int index =table->Alloc(item);
		printf("%s in:%d %d\n", currentThread->getName(), index, *(int*)item);
		t_msg->V();
		//currentThread->Yield();
	}

}

// TableConsumer
void Table_consumer(int which)
{
	void* item;
	while(true)
	{
		int index=0;
		//查找一个可用的表内容

		t_msg->P();
		for(;index < TABLESIZE;index++)
		{
			item = table->Get(index);
			if(item){
				break;
			}
		}
		printf("%s out:%d %d\n",currentThread->getName(), index, *(int*)item);
		table->Release(index);
		t_buf->V();
	}
}

// DllistProducer
void Dllist_producer(int which)
{

		// produce
		int *item = new int;
		int key = Random() % 100;
		*item = -key;
		d_mutex->Acquire();
		printf("Thread %d:%s in %d %d,\n",which,currentThread->getName(),key,*item);
		while(count >= N)
		{
			printf("Thread block : dlist is full...\n");
			printf("list: ");
			list->show();
			printf("\n");
			d_notFull->Wait(d_mutex);
			printf("Thread %d:%s in %d %d,\n",which,currentThread->getName(),key,*item);
		}
		list->SortedInsert((void*)item,key);
		count++;
		printf("list: ");
		list->show();
		printf("\n");
		d_notEmpty->Signal(d_mutex);
		d_mutex->Release();

}

void Dllist_consumer(int which)
{

		int key;
		int *item;
		d_mutex->Acquire();
		printf("Thread %d:%s dllist[0] out,\n",which,currentThread->getName());
		while(count==0){
			printf("Thread block : dlist is empty..\n");
			printf("list: ");
			list->show();
			printf("\n");
			d_notEmpty->Wait(d_mutex);
			printf("Thread %d:%s dllist[0] out,\n",which,currentThread->getName());
		}
		// take;
		item = (int*)list->Remove(&key);
		count--;
		d_notFull->Signal(d_mutex);
		printf("list: ");
		list->show();
		printf("\n");
		d_mutex->Release();
}


// boundbuffer producer
void BoundBuffer_producer(int which)
{
	while(TRUE)
	{
		//producer
		char * data = new char[b_in_size+1]();
		for(int i = 0; i < b_in_size; i++)
		{
			data[i] = Random()%26+'a';
		}
		boundbuffer->Write(data,b_in_size);
		printf("%s in: %s\n",currentThread->getName(),data);
		delete [] data;
	}
}

void BoundBuffer_consumer(int which)
{
	while(TRUE)
	{
		//consumer
		char *data = new char[b_out_size+1]();
		boundbuffer->Read(data,b_out_size);
		printf("%s out: %s \n",currentThread->getName(),data);
		delete [] data;
	}
}

void EventBarrierTest(int which)
{
    if(0==which)
    {
        currentThread->Yield();
        printf("thread %d id signal\n",which);
        barr->Signal();
    }
    else
    {
        printf("thread %d is waiting\n",which);
        barr->Wait();
        printf("thread %d is complete\n",which);
        barr->Complete();
        printf("Rhread %d is going\n",which);

    }

}

void AlarmTest(int which)
{
	// printf("*\n");
	// printf("%d \n",alarm_time_list[which]);
	printf("%s will sleep %d Ticks\n",currentThread->getName(),alarm_time_list[which]);
	alarm->Pause(alarm_time_list[which]);
	printf("%s will wakeup\n",currentThread->getName());
}

void ElevatorTest1(int which)
{
    int dir = UP;
    if (src[which] == des[which])
    {
	printf("This thread needn't elevator\n");	
	return; 
    } 
    else
    {
	do 
	{ 
	    printf("Thread %d travel from %d to %d\n",which,src[which],des[which]); 
            if (src[which] < des[which]) 
	    { 
                printf("Thread %d CallUp Elevator to Floor %d\n", which, src[which]);          	
				build->CallUp(src[which]);
                build->AwaitUp(src[which]);
			dir = UP;
            } 
            else 
	    { 
 	        printf("Thread %d CallDown Elevator to Floor %d\n", which, src[which]); 
			build->CallDown(src[which]); 
	  		build->AwaitDown(src[which]);
			dir = DOWN;
            } 
	    	currentThread->Yield(); 

        } while (!build->elevator->Enter(dir)); // elevator might be full! 
    	
		printf("Thread %d Enter Elevator\n", which); 
		build->elevator->RequestFloor(des[which]); 
		printf("Thread %d Exit Elevator\n", which); 
		build->elevator->Exit(); 
    }
    
}

void ElevatorTest2(int which)
{
    build->Begin();
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------


void ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1 ");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

void ThreadTest2()
{
	DEBUG('t', "Entering ThreadTest2 ");
	list=new DLList();
	sleep_lock=new Lock("sleep lock");
	for (int var = 0; var < T; var++)
	{
		char No[4]="1";
		sprintf(No, "%d", var);
		//char name[18]="forked thread ";	//error
		char *name=new char[25];			//must allocate new space,or the new thread will cover the address of 'name'
		name[0]='\0';
		strcat(name,"forked thread ");
		strcat(name,No);

		Thread *test = new Thread(name);
		test->Fork(TestDllist,var);//read to run this,and set the params for this procedure
	}
}

void ThreadTest3()
{
	DEBUG('t',"Entering ThreadTest3\n");
	table=new Table(TABLESIZE);
	t_buf = new Semaphore("table space",TABLESIZE);
	t_msg = new Semaphore("table have msg",0);
	Thread *p = new Thread("producer");
	p->Fork(Table_producer,0);
	Thread *c = new Thread("consumer");
	c->Fork(Table_consumer,1);

}

void ThreadTest4()
{
	list = new DLList;
	d_mutex = new Lock("dllist mutex");
	d_notEmpty=new Condition("dllist notEmpty");
	d_notFull = new Condition("dllist notFull");
	count=0;
	Thread * ptr1[3];
	Thread * ptr2[3];
	for(int i = 0;i < 3;i++)
	{
	    ptr1[i] = new Thread("dllist producer");
	}
	for(int j = 0;j < 3;j++)
	{
	    ptr2[j] = new Thread("dllist consumer");
	}
	ptr1[0]->Fork(Dllist_producer,0);
	ptr1[1]->Fork(Dllist_producer,1);
	ptr1[2]->Fork(Dllist_producer,2);
	ptr2[0]->Fork(Dllist_consumer,3);
	ptr2[1]->Fork(Dllist_consumer,4);
	ptr2[2]->Fork(Dllist_consumer,5);
}

void ThreadTest5()
{
	boundbuffer = new BoundedBuffer(b_maxsize);
	Thread *p = new Thread("producer");
	Thread *c = new Thread("consumer");

	p->Fork(BoundBuffer_producer,0);
	c->Fork(BoundBuffer_consumer,1);
}

void ThreadTest6()
{
    int i;
    DEBUG('t',"Entering ThreadTest6\n");
	// 先进入若干个进程，但是这些进程需要等待进程0
    for(i=1;i<=6;i++)
    {
		// sleep_lock->Acquire();
		char *msg = new char[50]();
		sprintf(msg,"EventBarrier sample thread %d",i);
		// sleep_lock->Release();
        Thread *t=new Thread(msg);
		// delete[] msg;
        t->Fork(EventBarrierTest,i);
        // currentThread->Yield();
    }
    EventBarrierTest(0);

}

void ThreadTest7()
{
	// Alarm test
	for(int i = 0; i < alarm_T; i++)
	{

		char *msg = new char[50]();
		sprintf(msg,"alarm thread %d",i);
		Thread* t = new Thread(msg);
		t->Fork(AlarmTest,i);


	}
}

void ThreadTest8()
{
    Thread * ptr[5];
    Thread * ptr_all;
    for(int i = 0; i < 5; i++)
    {
	char *msg = new char[50]();
	sprintf(msg,"Elevator thread %d",i);
	ptr[i] = new Thread(msg);
	ptr[i]->Fork(ElevatorTest1,i);
    }
    
    char *msg = new char[50]();
    sprintf(msg,"Building thread 5");
    ptr_all = new Thread(msg);
    ptr_all->Fork(ElevatorTest2,5);

}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest()
{
    switch (testnum) {
    case 1: // simpleThread
	ThreadTest1();
	break;
    case 2: // lab1 mutex ,default N=2 ,T=2
    ThreadTest2();
    break;
	case 3: // table producer consumer,tablesize=10
	ThreadTest3();
	break;
	case 4: // dllist producer consumer, default N=2
	ThreadTest4();
	break;
	case 5: // dllist producer consumer, default b_in_size=2,b_out_size=2,max_size=10
	ThreadTest5();
	break;
	case 6:
	ThreadTest6(); // EventBarrier test
	break;
	case 7:
	ThreadTest7(); // Alarm test , default alarm_T = 5,alarm_time_list = {1,2,3,4,5}
	break;
        case 8:
	ThreadTest8(); // Elevator test , default alarm_T = 5,alarm_time_list = {1,2,3,4,5}
        break;
    default:
	printf("No test specified.\n");
	break;
    }
}

