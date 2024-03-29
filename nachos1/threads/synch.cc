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
#include "synch.h"
#include "system.h"

#ifdef CHANGED
#include <unistd.h>
#include <sys/types.h>
#endif
//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new(std::nothrow) List;
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
#ifdef CHANGED

//----------------------------------------------------------------------
// Lock::Lock
//     Initializes a lock to be used for synchronization
//     key default should be FREE
//     
//----------------------------------------------------------------------

Lock::Lock(const char* debugName) {
  name = debugName;
  key = FREE;
  queue = new(std::nothrow) List;
}

//----------------------------------------------------------------------
// Lock::~Lock
//     Deallocates the lock. Assumes noone is still using the lock
//----------------------------------------------------------------------

Lock::~Lock() {
  delete queue;
}

//----------------------------------------------------------------------
// Lock::Acquire
//     Waits until the lock is free, then sets it to busy
//     mimics Semaphore::P in structure, except that 
//     key is only set to BUSY because it does not have an int value
//----------------------------------------------------------------------

void Lock::Acquire() {
  // the current thread should not already have the lock
  //  ASSERT(!isHeldByCurrentThread());
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  
  // the current thread should sleep until key is FREE
  while (key == BUSY) {
    queue->Append((void *)currentThread);
    currentThread->Sleep();
  }    

  key = BUSY;

  // this enables checking that the current thread has the lock
  threadWithLock = currentThread;
  (void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Lock::Release
//     Sets Lock to FREE, waking up a waiting thread if one is waiting
//     Mimics Semaphore::V in structure, except that 
//     key is only set to FREE because it does not have an int value
//----------------------------------------------------------------------

void Lock::Release() {
  Thread * thread;
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  
  // if the current thread does not have the lock, it can't release it
  while (!isHeldByCurrentThread())
    currentThread->Sleep();
  thread = (Thread *)queue->Remove();
  
  if (thread != NULL) {
    scheduler->ReadyToRun(thread);
  }
  
  // make key available
  key = FREE;
  
  // the current thread no longer has the lock
  threadWithLock = NULL;
  (void) interrupt->SetLevel(oldLevel);
  
}


//----------------------------------------------------------------------
// Lock::isHeldByCurrentThread
//     returns whether the current thread is the one that holds the lock
//----------------------------------------------------------------------

bool Lock::isHeldByCurrentThread(){
  return currentThread == threadWithLock;
}

//----------------------------------------------------------------------
// Condition::condition
//     Initializes a Condition, so that it can be used for synchronization
//----------------------------------------------------------------------

Condition::Condition(const char* debugName) { 
  name = debugName;
  queue = new(std::nothrow) List;
}

//----------------------------------------------------------------------
// Condition::~Condition
//     Deallocates a Condition
//----------------------------------------------------------------------

Condition::~Condition() { 
  delete queue;
}

//----------------------------------------------------------------------
// Condition::Wait
//     Release the lock, relinquish CPU control, and then 
//     reaquire the lock on waking up
//
//     conditionLock is the Lock that the condition waits on.
//----------------------------------------------------------------------

void Condition::Wait(Lock* conditionLock) { 
  // if current thread doesn't have lock, it can't wait on it
  ASSERT(conditionLock->isHeldByCurrentThread());
  IntStatus oldLevel = interrupt->SetLevel(IntOff);

  // release the lock on sleep
  conditionLock->Release();

  // put current thread on sleep list
  queue->Append((void *) currentThread);
  currentThread->Sleep();
  
  //reacquire the lock on waking up
  conditionLock->Acquire();

  (void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Condition::Signal
//     Wake up a thread if there are any waiting on the condition
//     
//     conditionLock is the Lock that the condition waits on.
//----------------------------------------------------------------------

void Condition::Signal(Lock* conditionLock) { 

  Thread * thread;
  IntStatus oldLevel = interrupt->SetLevel(IntOff);

  // wake up the first thread on the sleeping list
  thread = (Thread *)queue->Remove();
  if (thread != NULL)
    scheduler->ReadyToRun(thread);
  (void) interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// Condition:: Broadcast
//     Wake up all threads that are waiting on the lock.
//
//     conditionLock is the Lock that the condition waits on.
//----------------------------------------------------------------------

void Condition::Broadcast(Lock* conditionLock) { 

  Thread * thread;
  IntStatus oldLevel = interrupt->SetLevel(IntOff);

  // retrieve the first thread on the sleeping list
  thread = (Thread * )queue->Remove();

  // goes through all threads that are sleeping on this condition
  // and wakes them up
  while (thread != NULL) {
    scheduler->ReadyToRun(thread);
    thread = (Thread *) queue->Remove();    
  }
  (void) interrupt->SetLevel(oldLevel);
}

#endif
