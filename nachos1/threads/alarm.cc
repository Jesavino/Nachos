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
  mutex->Acquire();
  struct SCB * mySCB;
  
  mySCB = new SCB;
  mySCB->go = new(std::nothrow) Condition("SCB condition");
  mySCB->wakeTime = systime + howLong;
  //insert
  insert(mySCB);
  mySCB->go->Wait(mutex);
  printf("woke up\n");
  //remove first itme from lsit
  remove();
  delete mySCB;
  if ( list != NULL && list->wakeTime <= systime) 
      list->go->Signal(mutex);
  printf("about to release\n");
  mutex->Release();
}

void Alarm::insert(struct SCB * scb) {
  struct SCB * head = list;
  struct SCB * prev = head;
  if (head == NULL ) {
    list = scb;
    return;
  }
  else {
    while (scb->wakeTime < head->wakeTime && head != NULL) {
      prev = head;
      head = head->next;
    }
  }
  scb->next = head;
  prev->next = scb;
}

void Alarm::remove() {
  list = list->next;
}


void tick(int ) {
  systime++;
  if ( list != NULL && list->wakeTime <= systime) 
      list->go->Signal(mutex);
}

void perAlarm(int howLong) {
  //printf("in peralarm thread\n");
  Alarm * alarm = new(std::nothrow) Alarm("alarm");
  alarm->GoToSleepFor(howLong);
  //printf("%llu\n", stats->totalTicks);
}

void alarmTestStart(int numAlarms) {
  printf("in alarmTest\n");
  
  int i;
  Thread * alarm[numAlarms];
  for (i = 0; i < numAlarms; i++) {
    sprintf(strAlarm, "%s%d", "Alarm", i);
    alarm[i] = new(std::nothrow) Thread(strAlarm);
    alarm[i]->Fork(perAlarm, 1);
  }
}



#endif
