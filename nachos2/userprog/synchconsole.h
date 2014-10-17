#ifdef CHANGED
#ifndef __SYNCHCONSOLE_H__
#define __SYNCHCONSOLE_H__
#include "console.h"
#include "synch.h"

class SynchConsole{
 public:
  SynchConsole(char * in, char * out);
  
  ~SynchConsole();
  
  void SynchPutChar(char ch);
  
  char SynchGetChar();

 private:
  
  Console * console;
  Semaphore *readAvail;
  Semaphore *writeDone;

};



#endif //__SYNCHCONSOLE_H__
#endif // CHANGED
