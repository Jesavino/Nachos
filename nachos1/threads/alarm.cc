#ifdef CHANGED
#include "alarm.h"

char strAlarm[12];

Alarm::Alarm(const char * debugName) {
  Debug('t', "in alarm");
  name = debugName;
}

Alarm::~Alarm() {

}

void Alarm::GoToSleepFor(int howLong) {
  debug('t', "in Alarm::GoToSleepFor()");

}

void perAlarm(int howLong) {
  printf("in peralarm thread\n");
  Alarm * alarm = new(std::nothrow) Alarm("alarm");
  alarm->GoToSleepFor(100);
  
}

void alarmTestStart(int numAlarms) {
  printf("in alarmTest\n");
  
  int i;
  Thread * alarm[numAlarms]
  for (i = 0; i < numAlarms; i++) {
    sprintf(strAlarm, "%s%d", "Alarm", i);
    alarm[i] = new(std::nothrow) Thread(strAlarm);
    alarm[i]->Fork(perAlarm, 1);
}




#endif
