#ifdef CHANGED
#include "alarm.h"

char strAlarm[12];
Lock * alarmMutex = new(std::nothrow) Lock("AlarmLock");
long long unsigned  systime = 0;
unsigned int seed;
int totalRun;
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
  delete alarmMutex;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void Alarm::GoToSleepFor(int howLong) {
  DEBUG('t', "in Alarm::GoToSleepFor()");
  alarmMutex->Acquire();

  struct SCB * mySCB;
  
  mySCB = new SCB;
  mySCB->go = new(std::nothrow) Condition("SCB condition");
  mySCB->wakeTime = systime + (long long unsigned) howLong;

  printf("in GoToSleepFor %s: sleeping for %d ticks at systime %llu waketime: %llu\n", 
	 name, howLong, systime, mySCB->wakeTime);
  insert(mySCB);
  mySCB->go->Wait(alarmMutex);
  //  remove();
  printf("woke up %s at %llu: Slept for %llu ticks\n", name, systime, 
	 systime - (mySCB->wakeTime - (long long unsigned) howLong));

  delete mySCB->go;
  delete mySCB;

  while (list != NULL && list->wakeTime <= systime) {
    list->go->Signal(alarmMutex);
    list = list->next;
  }
  printf("%d threads have completed\n", ++totalRun);

  alarmMutex->Release();
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
  while (list != NULL && list->wakeTime <= systime) {
    list->go->Signal(alarmMutex);
    list = list->next;
    //    printf("in Tick()\n");
  }
  
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

void perAlarm(int howLong) {
  //printf("in peralarm thread\n");
  char alarmStr[12];
  sprintf(alarmStr, "%s %d", "Alarm", howLong);
  Alarm * alarm = new(std::nothrow) Alarm(alarmStr);
  unsigned int * seedp = &seed;
  int sleepTime = (rand_r(seedp) % 1000);
  alarm->GoToSleepFor(sleepTime);
  //printf("%llu\n", stats->totalTicks);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void alarmTestStart(int numAlarms) {
  printf("in alarmTest\n");
  
  int i;
  seed = 1;
  totalRun = 0;
  Thread * alarm[numAlarms];

  for (i = 0; i < numAlarms; i++) {
    sprintf(strAlarm, "%s%d", "Alarm", i);
    alarm[i] = new(std::nothrow) Thread(strAlarm);
    alarm[i]->Fork(perAlarm, i);
  }
}



#endif
