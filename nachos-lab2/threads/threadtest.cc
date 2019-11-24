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


void InsertList(int N, DLList *list);
void RemoveList(int N, DLList *list);
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
	sleep_lock->Acquire();  
	InsertList(N,list);
	if(E==1){
		//1号错误
		currentThread->Yield();
	}
	RemoveList(N,list);
	sleep_lock->Release();
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

	while (TRUE)
	{
		// produce
		int *item = new int;
		int key = Random() % 100;
		*item = -key;
		d_mutex->Acquire();
		while(count>=N)
		{
			printf("wait notfull...\n");
			d_notFull->Wait(d_mutex);
		}
		list->SortedInsert((void*)item,key);
		printf("%s in %d %d\n",currentThread->getName(),key,*item);
		count++;
		printf("list: ");
		list->show();
		d_notEmpty->Signal(d_mutex);
		d_mutex->Release();
	}
	
}

void Dllist_consumer(int which)
{
	while(TRUE)
	{
		
		int key;
		int *item;
		d_mutex->Acquire();
		while(count==0){
			printf("wait notempty...\n");
			d_notEmpty->Wait(d_mutex);
		}
		// take;
		item = (int*)list->Remove(&key);
		count--;
		d_notFull->Signal(d_mutex);
		d_mutex->Release();
		printf("%s out %d %d\n",currentThread->getName(),key,*item);
		printf("\nlist: ");
		list->show();
	}
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
	
	Thread * p = new Thread("dllist producer");
	Thread * c = new Thread("dllist consumer");
	p->Fork(Dllist_producer,0);
	c->Fork(Dllist_consumer,1);
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
    default:
	printf("No test specified.\n");
	break;
    }
}

