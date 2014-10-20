#ifdef CHANGED
#ifndef __SYNCHCONSOLE_H__
#define __SYNCHCONSOLE_H__
#include "console.h"
#include "synch.h"

class SynchConsole{
 public:
  SynchConsole(char * in, char * out);
  
  ~SynchConsole();
  
  void ReadAvail();

  void WriteDone();
  
  void WriteLine(char * line);

  void SynchPutChar(char ch);
  
  char SynchGetChar();

 private:
  
  Console * console;
  Semaphore *readAvail;
  Semaphore *writeDone;
  Lock * mutex;
  void PrivatePutChar(char ch);
  char PrivateGetChar();
};



#endif //__SYNCHCONSOLE_H__
#endif // CHANGED
