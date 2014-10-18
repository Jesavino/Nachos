#ifdef CHANGED
#include "synchconsole.h"

//----------------------------------------------------------------------
// dummy functions because C++ can't handle pointers to member functions
//----------------------------------------------------------------------

static void readAvailFunc(int c) {
  SynchConsole * console = (SynchConsole *) c;
  console->ReadAvail();
}

static void writeDoneFunc(int c) {
  SynchConsole * console = (SynchConsole *) c;
  console->WriteDone();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

SynchConsole::SynchConsole(char * in, char * out){
  console = new(std::nothrow) Console(in, out, readAvailFunc, writeDoneFunc, (int) this);
  readAvail = new(std::nothrow) Semaphore("read available", 0);
  writeDone = new(std::nothrow) Semaphore("write done", 0);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

SynchConsole::~SynchConsole() {
  delete console;
  delete readAvail;
  delete writeDone;

}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void SynchConsole::ReadAvail() {
  readAvail->V();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void SynchConsole::WriteDone() {
  writeDone->V();
}

//----------------------------------------------------------------------
// write char to display
//----------------------------------------------------------------------

void SynchConsole::SynchPutChar(char ch) {
  console->PutChar(ch);	// echo it!
  writeDone->P();
}

//----------------------------------------------------------------------
// get char from input
//----------------------------------------------------------------------

char SynchConsole::SynchGetChar() {
  readAvail->P();		// wait for character to arrive
  
  char ch = console->GetChar();
  return ch;
}




#endif // CHANGED
