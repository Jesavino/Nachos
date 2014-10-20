#ifdef CHANGED
#ifndef __SYNCHCONSOLE_H__
#define __SYNCHCONSOLE_H__
#include "console.h"
#include "synch.h"

//----------------------------------------------------------------------
// SynchConsole class
//     provides synchronous access to the machine console.
//----------------------------------------------------------------------

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

  //private methods for actual access to the console
  // allows the calling methods to provide mutex, so that 
  // different methods may access the console.
  // e.g. WriteLine and SynchPutChar both can obtain mutex,
  // and then call the actual put method without deadlock
  void PrivatePutChar(char ch);
  char PrivateGetChar();
};



#endif //__SYNCHCONSOLE_H__
#endif // CHANGED
