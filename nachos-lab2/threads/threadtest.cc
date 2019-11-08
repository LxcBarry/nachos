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
// #include "synch-sem.h"

// testnum is set in main.cc
int testnum = 1;
int T=2;
int N=2;
int E=0;
DLList *	list;

// sleep lock
Lock * sleep_lock;


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
	// sleep_lock->Acquire();  
	InsertList(N,list);
	if(E==1){
		//1号错误
		currentThread->Yield();
	}
	RemoveList(N,list);
	// sleep_lock->Release();
	//printf("*** thread %d ***\n",which);
	if(list->IsEmpty())
	{
		printf("*** thread %d :empty ***\n",which);
	}

	
	currentThread->Finish(); // give back the space

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

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    case 2:
    ThreadTest2();
    break;
    default:
	printf("No test specified.\n");
	break;
    }
}

