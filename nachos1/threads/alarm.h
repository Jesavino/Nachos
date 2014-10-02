#ifdef CHANGED
#ifndef __ALARM_H__

class Alarm{
 public:
  Alarm(char * debugName);
  ~Alarm();
  void GoToSleepFor(int HowLong);



 private:
  char * name;
  int sysTime;
  int sleepTime;





#endif
#endif
