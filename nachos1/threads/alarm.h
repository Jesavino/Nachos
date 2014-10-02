#ifdef CHANGED
#ifndef __ALARM_H__
#include <stdio.h>

class Alarm{
 public:
  Alarm(const char * debugName);
  ~Alarm();
  void GoToSleepFor(int HowLong);



 private:
  const char * name;
  int sysTime;
  int sleepTime;




};
#endif
#endif
