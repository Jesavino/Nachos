#ifdef CHANGED
#include "alarm.h"

char strAlarm[12];
long long unsigned  systime = 0;
Lock * mutex = new(std::nothrow) Lock("stupid fucking lock");

struct SCB {
  Condition * go;
  long long unsigned wakeTime;
  struct SCB * next;
};

struct SCB * list;

//----------------------------------------------------------------------
//----------------------------------------------------------------------

Alarm::Alarm(const char * debugName) {
  DEBUG('t', "in alarm");
  name = debugName;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

Alarm::~Alarm() {
  delete mutex;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void Alarm::GoToSleepFor(int howLong) {
  DEBUG('t', "in Alarm::GoToSleepFor()");
  printf("in GoToSleepFor %s: sleeping for %d ticks at systime %llu\n", name, howLong, systime);

  mutex->Acquire();

  struct SCB * mySCB;
  
  mySCB = new SCB;
  mySCB->go = new(std::nothrow) Condition("SCB condition");
  mySCB->wakeTime = systime + (long long unsigned) howLong;

  //insert
  insert(mySCB);
  mySCB->go->Wait(mutex);
  printf("woke up %s at %llu: Slept for %llu ticks\n", name, systime, 
	 systime - (mySCB->wakeTime - (long long unsigned) howLong));

  //remove first item from list
  //  remove();
  delete mySCB;
  //  struct SCB * next = list;

  //  if ( next != NULL && next->wakeTime <= systime) {
  //   next->go->Signal(mutex);
       //     next = next->next;
  //}

  //printf("about to release %s\n", name);

  mutex->Release();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void Alarm::insert(struct SCB * scb) {
  struct SCB * head = list;

  if (head == NULL ) {
    list = scb;
    return;
  }

  else if (scb->wakeTime < list->wakeTime) {
    scb->next = list;
    list = scb;
  }

  else {
    for(head = list; head->next != NULL; head=head->next) {
      if (scb->wakeTime < head->next->wakeTime) {
	scb->next = head->next;
	head->next = scb;
	return;
      }
    }
    head->next = scb;
  }
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

void Alarm::remove() {
  list = list->next;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

void tick(int ) {
  systime = stats->totalTicks;
  
  struct SCB * next = list;

  //  if (next != NULL)
  // printf("here waketime: %llu systime: %llu\n",next->wakeTime, systime);
  while ( next != NULL && next->wakeTime <= systime) {
      next->go->Signal(mutex);
      
      next = next->next;
      //remove();
      list=list->next;
  }
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void perAlarm(int howLong) {
  //printf("in peralarm thread\n");
  char alarmStr[12];
  sprintf(alarmStr, "%s %d", "Alarm", howLong);
  Alarm * alarm = new(std::nothrow) Alarm(alarmStr);
  alarm->GoToSleepFor(howLong * 10 + 1);
  //printf("%llu\n", stats->totalTicks);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void alarmTestStart(int numAlarms) {
  printf("in alarmTest\n");
  
  int i;
  Thread * alarm[numAlarms];

  for (i = 0; i < numAlarms; i++) {
    sprintf(strAlarm, "%s%d", "Alarm", i);
    alarm[i] = new(std::nothrow) Thread(strAlarm);
    alarm[i]->Fork(perAlarm, i);
  }
}



#endif
