// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch-sem.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{

    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!

//binary lock
Lock::Lock(char* debugName){
    name=debugName;
    value = 1; //value=0--lock is busy value=1--lock is avilable
    LockQueue = new List;
    LockHolder = NULL;
}
Lock::~Lock() {
    delete (LockQueue);
}
void Lock::Acquire() {
    IntStatus oldLevel=interrupt->SetLevel(IntOff);
    while (!value)
    {
        LockQueue->Append((void*)currentThread);
        currentThread->Sleep();
    }
    value=0;
    LockHolder=currentThread;

    (void)interrupt->SetLevel(oldLevel);    

}
void Lock::Release() {
    Thread* t;
    ASSERT(isHeldByCurrentThread());
    IntStatus oldLevel=interrupt->SetLevel(IntOff);

    if(!LockQueue->IsEmpty()){
        t=(Thread*)LockQueue->Remove();
        if(t) scheduler->ReadyToRun(t);
    }
    else{
        value=1;

    }
    LockHolder=NULL;
    
    (void)interrupt->SetLevel(oldLevel);
}

bool Lock::isHeldByCurrentThread(){
    return LockHolder == currentThread;
}

Condition::Condition(char* debugName) { 
    name = debugName;
    // lock = new Lock("condition lock");
    numWaiting = 0;
    queue = new List;
    holder = NULL;
}
Condition::~Condition() {
    delete (queue);
    // delete (lock);
 }
void Condition::Wait(Lock* conditionLock) { 
    ASSERT(conditionLock->isHeldByCurrentThread()); 
    IntStatus oldLevel=interrupt->SetLevel(IntOff);
    DEBUG('s',"condition waiting...\n");
    numWaiting++;
    queue->Append((void*)currentThread);
    conditionLock->Release();
    // go to sleep
    currentThread->Sleep();
    DEBUG('s',"condition wakeup...\n");
    conditionLock->Acquire();
    (void)interrupt->SetLevel(oldLevel);
}
void Condition::Signal(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread()); 
    IntStatus oldLevel=interrupt->SetLevel(IntOff);
    DEBUG('s',"condition signal...\n");
    if(numWaiting > 0){
        Thread *t = (Thread*)queue->Remove();
        if(t) scheduler->ReadyToRun(t);
        numWaiting--;
    }
    (void)interrupt->SetLevel(oldLevel);
 }

void Condition::Broadcast(Lock* conditionLock) { 
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel=interrupt->SetLevel(IntOff);
    DEBUG('s',"condition broadcase");
    Thread *t=NULL;
    t=(Thread*)queue->Remove();
    while(t){
        scheduler->ReadyToRun(t);
        t=(Thread*)queue->Remove();
    }
    numWaiting=0;
    (void)interrupt->SetLevel(oldLevel);
}
