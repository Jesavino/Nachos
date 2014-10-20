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
  mutex = new(std::nothrow) Lock("synchconsole mutex");
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

SynchConsole::~SynchConsole() {
  delete console;
  delete readAvail;
  delete writeDone;
  delete mutex;

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
//----------------------------------------------------------------------

void SynchConsole::WriteLine(char * line) {
  mutex->Acquire();
  int i = 0;
  while (line[i] != '\0') {
    PrivatePutChar(line[i]);
    i++;
  }
  mutex->Release();
}

//----------------------------------------------------------------------
// write char to display
//----------------------------------------------------------------------

void SynchConsole::SynchPutChar(char ch) {
  mutex->Acquire();
  PrivatePutChar(ch);
  mutex->Release();
}

//----------------------------------------------------------------------
// get char from input
//----------------------------------------------------------------------

char SynchConsole::SynchGetChar() {
  mutex->Acquire();

  char ch = PrivateGetChar();

  mutex->Release();
  return ch;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void SynchConsole::PrivatePutChar(char ch) {
  console->PutChar(ch);
  writeDone->P();
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

char SynchConsole::PrivateGetChar() {

  readAvail->P();		// wait for character to arrive
  char ch = console->GetChar();
  return ch;
}

#endif // CHANGED
