#ifdef CHANGED
#include "alarm.h"

char strAlarm[12];
int systime = 0;
Lock * mutex;
struct SCB {
  Condition * go;
  int wakeTime;
  struct SCB * next;
};

struct SCB * list;

Alarm::Alarm(const char * debugName) {
  DEBUG('t', "in alarm");
  name = debugName;
  mutex = new(std::nothrow) Lock("mutex");
}

Alarm::~Alarm() {

}

void Alarm::GoToSleepFor(int howLong) {
  DEBUG('t', "in Alarm::GoToSleepFor()");
  printf("in GoToSleepFor %s: sleeping for %d ticks\n", name, howLong);
  mutex->Acquire();
  struct SCB * mySCB;
  
  mySCB = new SCB;
  mySCB->go = new(std::nothrow) Condition("SCB condition");
  mySCB->wakeTime = systime + howLong;
  //insert
  insert(mySCB);
  mySCB->go->Wait(mutex);
  printf("woke up %s: Slept for %d ticks\n", name, systime - (mySCB->wakeTime - howLong));
  //remove first itme from lsit
  remove();
  delete mySCB;
  struct SCB * next = list;
  while ( next != NULL && next->wakeTime <= systime) {
      next->go->Signal(mutex);
      next = next->next;
  }
  printf("about to release %s\n", name);
  mutex->Release();
}

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


void Alarm::remove() {
  list = list->next;
}


void tick(int ) {
  systime += 100;
  struct SCB * next = list;
  while ( next != NULL && next->wakeTime <= systime) {
      next->go->Signal(mutex);
      next = next->next;
  }
}

void perAlarm(int howLong) {
  //printf("in peralarm thread\n");
  char alarmStr[12];
  sprintf(alarmStr, "%s %d", "Alarm", howLong);
  Alarm * alarm = new(std::nothrow) Alarm(alarmStr);
  alarm->GoToSleepFor(howLong + 10);
  //printf("%llu\n", stats->totalTicks);
}

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
