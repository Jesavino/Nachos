#ifdef CHANGED
#include "synchconsole.h"

SynchConsole::SynchConsole(int in, int out){
  console = new(std::nothrow) Console(in, out, NULL,NULL/*func ptrs*/, 0);
  readAvail = new(std::nothrow) Semaphore("read available", 0);
  writeDone = new(std::nothrow) Semaphore("write done", 0);
}

SynchConsole::~SynchConsole() {
  delete console;
  delete readAvail;
  delete writeDone;

}

void SynchConsole::SynchPutChar(char ch) {

}

char SynchConsole::SynchGetChar() {

}




#endif // CHANGED
