#ifdef CHANGED
#include "alarm.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//     Constructor for the alarm class
//----------------------------------------------------------------------

Alarm::Alarm(const char * debugName) {
  DEBUG('t', "in alarm");
  name = debugName;
}

//----------------------------------------------------------------------
// Alarm::~Alarm()
//     Destructor for the alarm class. 
//     Alarm is simple enough that nothing really needs destroyed
//----------------------------------------------------------------------

Alarm::~Alarm() {
}

//----------------------------------------------------------------------
// Alarm::GoToSleepFor(int howLong)
//     puts the alarm to sleep for howLong ticks. 
//     when it awakes, it determines if another thread needs to wake.
//----------------------------------------------------------------------

void Alarm::GoToSleepFor(int howLong) {
  DEBUG('t', "in Alarm::GoToSleepFor()");
  
  // load the struct with information 
  struct SCB * mySCB;
  
  mySCB = new SCB;
  // each struct needs its own condition variable so that when we want to 
  // wake the alarm, we only need to signal its condition variable.
  mySCB->go = new(std::nothrow) Condition("SCB condition");

  // waketime will be current time plus the amount of time we should sleep
  mySCB->wakeTime = systime + (long long unsigned) howLong;

  // if this is not NULL it can generate a segfault later on
  mySCB->next = NULL;
  printf("*** in GoToSleepFor %12s: sleeping for %4d ticks at systime %6llu waketime: %6llu\n", 
	 name, howLong, systime, mySCB->wakeTime);

  // acquire the mutex for struct operations. 
  alarmMutex->Acquire();

  // insert the thread into the linked list in the order of when it should wake up.
  insert(mySCB);

  // and then wait on this alarms condition variable.
  mySCB->go->Wait(alarmMutex);

  //the item at the head of the list should have awoken, which is now this alarm
  // we have mutex, so this alarm can be removed from the list.
  remove();

  // information for showing the status of the alarm.
  printf("*** woke up %12s at %6llu: Slept for %6llu ticks", name, systime, 
	 systime - (mySCB->wakeTime - (long long unsigned) howLong));
  printf(" *** %3d threads have completed\n", ++totalRun);

  // signal an alarm if it was supposed to wake at the same time
  if (list != NULL && list->wakeTime <= systime) {
    list->go->Signal(alarmMutex);
  }

  // release the mutex after all list operations are done.
  alarmMutex->Release();

  // delete the struct data.
  delete mySCB->go;
  delete mySCB;


}

//----------------------------------------------------------------------
// Alarm::insert(struct SCB * scb) 
//     puts an item into a list in order of increasing wakeTime
//     operates on a global struct
//     emulates the List::SortedRemove() function 
//----------------------------------------------------------------------

void Alarm::insert(struct SCB * scb) {
  struct SCB * head = list;

  // if there is nothing on the list
  if (head == NULL ) {
    list = scb;
    return;
  }

  // if there is something on the list, but we should insert
  // this one at the beginning
  else if (scb->wakeTime < list->wakeTime) {
    scb->next = list;
    list = scb;
  }
  
  // insert the element at the correct location in the list
  else {
    for(head = list; head->next != NULL; head=head->next) {
      if (scb->wakeTime < head->next->wakeTime) {
	scb->next = head->next;
	head->next = scb;
	// if it was inserted, we can exit the function
	return;
      }
    }
    // if the element is supposed to be placed last.
    head->next = scb;
  }
}


//----------------------------------------------------------------------
// Alarm::remove()
//     removes the first item from the list.
//----------------------------------------------------------------------

void Alarm::remove() {
  list = list->next;
}


//----------------------------------------------------------------------
// tick(int )
//     a timer interrupt handler for a timer created to aid the alarm class
//     its declaration is modeled after the original timer interrupt handler
//     assigns the current running time to systime, then
//     checks to see if any alarms need to wake up
//----------------------------------------------------------------------

void tick(int ) {

  systime = stats->totalTicks;

  // if an item is supposed to wake up at this point in time, 
  // signal it.
  if ( list != NULL && list->wakeTime <= systime) {
    list->go->Signal(alarmMutex);
  }
  
}

//----------------------------------------------------------------------
// foreverYield(int i)
//     loops a thread forever so the Nachos system does not quit while
//     waiting for alarms to wake up.
//     parameter i is unnecessary, the function just needs to accept an
//     argument as per Nachos standards
//----------------------------------------------------------------------

void foreverYield(int i) {
  while (1) 
    keepAlive->Yield();
}

//----------------------------------------------------------------------
// perAlarm(int threadNum)
//     a helper function for the alarm test.
//     it creates a new alarm for each thread, assigns a random number 
//     to sleepTime, and calls Alarm::GoToSleepFor(sleepTime),
//     putting each thread to sleep until it is supposed to wake up.
//     threadNum is an incremented number that effectively serves
//     to help us identify alarms 
//----------------------------------------------------------------------

void perAlarm(int threadNum) {
  char alarmStr[12];

  // create the object debug name
  sprintf(alarmStr, "%s %d", "Alarm", threadNum);
  Alarm * alarm = new(std::nothrow) Alarm(alarmStr);

  // assign a pointer to the location of the seed.
  unsigned int * seedp = &seed;

  // items will sleep somewhere in the range [0, MAXSLEEP]
  int sleepTime = (rand_r(seedp) % MAXSLEEP);

  // put each alarm to sleep
  alarm->GoToSleepFor(sleepTime);
}

//----------------------------------------------------------------------
// alarmTestStart(int numAlarms)
//     entry point for the alarm test suite.
//     creates a number of threads that were specified on the command line
//     and forks each one off to the function perAlarm()
//     then it creates a thread that does nothing but yield in an infinite loop
//     ensuring that the system will not quit while waiting for alarms to wake.
//----------------------------------------------------------------------

void alarmTestStart(int numAlarms) {  
  
  // assign the seed for the amount of time each alarm will sleep.
  // can be changed for different behavior
  seed = 1;
  
  // used for statistic purposes
  totalRun = 0;

  // create an array of alarms, for easy forking.
  Thread * alarm[numAlarms];

  int i;

  // fork each alarm
  for (i = 0; i < numAlarms; i++) {
    sprintf(strAlarm, "%s%d", "Alarm", i);
    alarm[i] = new(std::nothrow) Thread(strAlarm);
    alarm[i]->Fork(perAlarm, i);
  }

  // thread to keep system alive while things sleep
  keepAlive = new(std::nothrow) Thread("keepAlive");
  keepAlive->Fork(foreverYield, 0);
}



#endif
