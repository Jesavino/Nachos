#ifdef CHANGED
#ifndef __GROUPTEST_H__
#define __GROUPTEST_H__

#include <iostream>
#include "synch.h"
#include <string.h>

class LockTest {
public:
  LockTest();

  void consumerThread(int buffer);
  void producerThread(int buffer);

private:
  Condition *full;
  Condition *empty;
  Lock *pLock;
  Lock *cLock;
  int n, m, i, size, inBuffer;;

  
  void testStart();
};

#endif
#endif
