#ifdef CHANGED
#ifndef __ALARM_H__
#include <stdio.h>
#include "list.h"
#include "thread.h"
#include <new>
#include "stats.h"
#include "synch.h"
#include <stdlib.h>

// to get the totalTicks, ie the system running time
extern Statistics * stats;

// for giving each thread a name.
char strAlarm[12];

// a mutex for access to the linked list (below) of alarms. 
// needs to be global because the timer interrupt handler tick()
// signals conditions with the mutex.
Lock *  alarmMutex = new(std::nothrow) Lock("AlarmLock");

// easier to type than stats->totalTicks for use in various statements.
long long unsigned  systime = 0;

// so we can potentially change the randomness
// of the times the alarms sleep for
unsigned int seed;

// used for debugging, or for testing, shows how many alarms have run
// can compare to how many we called on the command line to show correctness
int totalRun;

// the maximum amount of time any alarm will sleep
int MAXSLEEP = 5000;
// A thread that yields in an infinite loop.
// keeps Nachos alive
Thread * keepAlive;

// a simple linked list to keep track of which alarms need to wake up.
// when used in conjunction with Alarm::Insert() and Alarm::Remove()
// keeps the list sorted according to wakeTime in increasing order.
struct SCB {
  Condition * go;
  long long unsigned wakeTime;
  struct SCB * next;
};

struct SCB * list;


// A simple Alarm class that puts the current thread to sleep for 
// a specified amount of time. 
class Alarm{
 public:
  // constructor
  Alarm(const char * debugName);

  //destructor
  ~Alarm();

  // put thread to sleep for HowLong
  void GoToSleepFor(int HowLong);

  // insert method for linked list struct
  void insert(struct SCB * scb);

  // remove method for linked list struct
  void remove();
  

 private:
  // name for debugging
  const char * name;

};
#endif
#endif
