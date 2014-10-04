#ifdef CHANGED
#include "alarm.h"

char strAlarm[12];
int systime = 0;
Condition * go;
Lock * mutex;
struct SCB {
  int wakeTime;
  struct SCB * next;
};

struct SCB *list;

Alarm::Alarm(const char * debugName) {
  DEBUG('t', "in alarm");
  name = debugName;
  mutex = new(std::nothrow) Lock("mutex");
  go = new(std::nothrow) Condition("go condition");
}

Alarm::~Alarm() {

}

void Alarm::GoToSleepFor(int howLong) {
  DEBUG('t', "in Alarm::GoToSleepFor()");
  printf("in gotosleep\n");
  mutex->Acquire();
  struct SCB * mySCB;
  
  mySCB = new SCB;
  mySCB->wakeTime = systime + howLong;
  //insert
  insert(mySCB);
  go->Wait(mutex);
  printf("woke up\n");
  //remove first itme from lsit
  remove();
  delete mySCB;
  if ( list != NULL) 
    if (list->wakeTime >= systime) go->Signal(mutex);
  mutex->Release();
}

void Alarm::insert(struct SCB * scb) {
  struct SCB * head = list;
  struct SCB * prev = head;
  if (head == NULL ) {
    head = scb;
    return;
  }
  while (scb->wakeTime < head->wakeTime && head != NULL) {
    prev = head;
    head = head->next;
  }
  scb->next = head;
  prev->next = scb;
}

void Alarm::remove() {
  list = list->next;
}

void perAlarm(int howLong) {
  printf("in peralarm thread\n");
  Alarm * alarm = new(std::nothrow) Alarm("alarm");
  alarm->GoToSleepFor(100);
  printf("%llu\n", stats->totalTicks);
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

void tick(int ) {
  systime++;
  if (list != NULL) 
    go->Signal(mutex);

}


#endif
