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
#include "child.h"
#include <stdio.h>
#include <stdlib.h>

Thread *t[20];      // The processes involved in barrier synch.
Semaphore *bed[20]; // Private semaphores for early processes.
Semaphore *mutex = new Semaphore("mutex",1);
int done=0;         // Number of processes at the barrier.
char pname[20][5];
char sname[20][5];

// Stuff related to commo with wish sub-process

FILE *read_from, *write_to;
int childpid;
char result[80], dummy;

void
gotback()
{
  if (fgets(result, 80, read_from) <= 0) exit(0); // exit if wish dies
  if (sscanf(result,"%c", &dummy) != 1) exit(0);   // exit if bogus response
}

void
atbarrier(int name)
{
  fprintf (write_to, "tobarrier %d\n", name);
}

void
toend(int name)
{
  fprintf (write_to, "toend %d\n", name);
}

void
todead(int name)
{
  fprintf (write_to, "todead %d\n", name);
}

void
BarrierProcess(int arg)
{
  int i, delay;

  // Random delay before the barrier.
  // Toggle interrupts from 1 to 300000 times.

  delay = 1+(int) (300000.0*rand()/(RAND_MAX+1.0));
  for (i=0; i<delay; i++){
    interrupt->SetLevel(IntOff);
    interrupt->SetLevel(IntOn);
  }

  // At the barrier.
  atbarrier(arg);
  gotback();

  mutex->P();
  if (++done != 20) {
    mutex->V();
    
    bed[arg]->P();
  }
  else {
    done = 0;
    for (i=0; i<20; i++) if (i != arg) bed[i]->V();
    mutex->V();
  }

  ASSERT(done==0); // blow up if barrier violation

  // Past the barier.
  toend(arg);
  gotback();

  // Random delay after the barrier.
  // Toggle interrupts from 1 to 300000 times.

  delay = 1+(int) (300000.0*rand()/(RAND_MAX+1.0));
  for (i=0; i<delay; i++){
    interrupt->SetLevel(IntOff);
    interrupt->SetLevel(IntOn);
  }

  // Finished.
  todead(arg);
  gotback();

}

void
ThreadTest()
{
  int i;

  DEBUG('t', "Starting the BARRIER System");

  // Instantiate the 20 private semaphores.
  for (i=0; i<20; i++) {
    sprintf(sname[i], "s%d", i);
    bed[i] = new Semaphore(sname[i],0);
  }

  // Bring up the display
  childpid = start_child("wish",&read_from,&write_to);

  // Tell wish to read the init script
  fprintf (write_to, "source barrier.tcl\n");
  gotback();

  // Instantiate the 20 threads.
  for (i=0; i<20; i++) {
    sprintf(pname[i], "p%d", i);
    t[i] = new Thread(pname[i]);
    t[i]->Fork(BarrierProcess,i);
  }
  // Having spawned 20 threads, the initial thread terminates.  
}

#endif
