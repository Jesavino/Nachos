#ifdef CHANGED
#ifndef __ALARM_H__
#include <stdio.h>
#include "list.h"
#include "thread.h"
#include <new>
#include "stats.h"
#include "synch.h"
#include <stdlib.h>

extern Statistics * stats;

class Alarm{
 public:
  Alarm(const char * debugName);
  ~Alarm();
  void GoToSleepFor(int HowLong);
  void insert(struct SCB * scb);
  void remove();
  int sysTime;



 private:
  const char * name;
  int sleepTime;
  int wakeTime;
};
#endif
#endif
