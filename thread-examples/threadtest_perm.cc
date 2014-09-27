#ifndef CHANGED
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

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}
#else

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <stdio.h>
#include <stdlib.h>
#include <new>

Thread *t1, *t2;
Semaphore *oktogo = new(std::nothrow) Semaphore("oktogo",0);

void
P1(int)
{
  int i, delay, count=0;

  // Random delay before giving/seeking permission to proceed.
  // Toggle interrupts from 1 to 100000 times.

  delay = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));
  printf("\nP1 delay is %d\n", delay); fflush(stdout);
  for (i=0; i<delay; i++){
    interrupt->SetLevel(IntOff);
    if (!(++count%100)) printf("1"); fflush(stdout);
    interrupt->SetLevel(IntOn);
  }

  printf("\nP1 giving permission\n"); fflush(stdout);
  oktogo->V();
}

void
P2(int)
{
  int i, delay, count=0;

  // Random delay before giving/seeking permission to proceed.
  // Toggle interrupts from 1 to 100000 times.

  delay = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));
  printf("\nP2 delay is %d\n", delay); fflush(stdout);
  for (i=0; i<delay; i++){
    interrupt->SetLevel(IntOff);
    if (!(++count%100)) printf("2"); fflush(stdout);
    interrupt->SetLevel(IntOn);
  }

  printf("\nP2 asking permission\n"); fflush(stdout);
  oktogo->P();
  printf("\nP2 has fired\n"); fflush(stdout);
}

void
ThreadTest()
{

  DEBUG('t', "Starting the PERMISSION TO PROCEED System");

  t1 = new(std::nothrow) Thread("p1");
  t2 = new(std::nothrow) Thread("p2");
  t1->Fork(P1,0);
  t2->Fork(P2,0);
}

#endif
