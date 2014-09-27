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

Thread *t[20];      // The processes involved in barrier synch.
Semaphore *bed[20]; // Private semaphores for early processes.
Semaphore *mutex = new(std::nothrow) Semaphore("mutex",1);
int done=0;         // Number of processes at the barrier.
char pname[20][5];
char sname[20][5];

char status[20];    // For displaying the state of the threads.
                    // status[i] == '-' means P(i) isn't at barrier yet
                    //           == 'B' means P(i) is at the barrier
                    //           == '*' means P(i) is past the barrier
                    //           == 'O' means P(i) has finished

void
statusprint()
{
  int i;

  printf(" ");
  for (i=0; i<20; i++) {
    printf("%c",status[i]);
  }
  printf("\n");
  fflush(stdout);
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

  status[arg] = 'B';
  statusprint();

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

  status[arg] = '*';
  statusprint();

  // Random delay after the barrier.
  // Toggle interrupts from 1 to 300000 times.

  delay = 1+(int) (300000.0*rand()/(RAND_MAX+1.0));
  for (i=0; i<delay; i++){
    interrupt->SetLevel(IntOff);
    interrupt->SetLevel(IntOn);
  }

  status[arg] = 'O';
  statusprint();
 
}

void
ThreadTest()
{
  int i;

  DEBUG('t', "Starting the BARRIER System");

  // Instantiate the 20 private semaphores.
  for (i=0; i<20; i++) {
    sprintf(sname[i], "s%d", i);
    bed[i] = new(std::nothrow) Semaphore(sname[i],0);
  }

  // Set up the display string.
  for (i=0; i<20; i++) {
    status[i] = '-';
  }

  printf("Key:\n");
  printf("\t- means P(i) isn't at barrier yet\n");
  printf("\tB means P(i) is at the barrier\n");
  printf("\t* means P(i) is past the barrier\n");
  printf("\tO means P(i) has finished\n\n\n");


  statusprint();

  // Instantiate the 20 threads.
  for (i=0; i<20; i++) {
    sprintf(pname[i], "p%d", i);
    t[i] = new(std::nothrow) Thread(pname[i]);
    t[i]->Fork(BarrierProcess,i);
  }
  // Having spawned 20 threads, the initial thread terminates.  
}

#endif
