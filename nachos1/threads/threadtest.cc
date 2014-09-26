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
#include <iostream>
#include "synch.h"
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

#ifdef CHANGED
class Locktest {
public:
  Locktest() {}
  void consumerThread(int buffer) {
    char * buff = (char *) buffer;
    
    while (1) {
      cLock->Acquire();
      if (inBuffer == 0) empty->Wait(cLock);
      std::cout << buff[i] << std::endl;
      inBuffer--;
      i = (i + 1) % size;
      cLock->Release();
      full->Signal(pLock);
      
    }
  }
  
  void producerThread(int buffer) {
    char * buff = (char *) buffer;
    char * hello = "Hello world";
    int size = 2;
    while (1){
      pLock->Acquire();
      if (inBuffer == size) full->Wait(pLock);
      buff[n] = hello[m];
      inBuffer++;
      n = (n + 1) % size;
      m = (m + 1);
      
      pLock->Release();
      empty->Signal(cLock);
      if (m >= strlen(hello)) break;
    }
    
  }
private:
  Condition full = new(std::nothrow) Condition("producer");
  Condition empty = new(std::nothrow) Condition("consumer");
  Lock pLock = new(std::nothrow) Lock("pLock");
  Lock cLock = new(std::nothrow) Lock("cLock");
  int n = 0, m = 0, i = 0;
  int size = 2, inBuffer = 0;
}
  
void LockTest() {
  DEBUG('t', "Entering Locktest\n");
  //create producer consumer
  char buffer[size];
  char * pbuffer = buffer;
  Thread * consumer = new(std::nothrow) Thread("consumer");
  Thread * producer = new(std::nothrow) Thread("producer");

  consumer->Fork(Locktest::consumerThread, (int ) pbuffer);
  Locktest::producerThread((int ) pbuffer);

}

#endif

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new(std::nothrow) Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

