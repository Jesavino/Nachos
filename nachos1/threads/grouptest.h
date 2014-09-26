#ifdef CHANGED
#ifndef __GROUPTEST_H__
#define __GROUPTEST_H__

#include <iostream>
#include "synch.h"

class Locktest {
public:
  Locktest();

  void consumerThread(int buffer);
  void producerThread(int buffer);

private:
  Condition full;
  Condition empty;
  Lock pLock;
  Lock cLock;
  int n, m, i, size, inBuffer;;

  
void LockTest();
}

#endif
#endif
