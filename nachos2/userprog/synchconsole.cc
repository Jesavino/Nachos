#ifdef CHANGED
#include "synchconsole.h"

//----------------------------------------------------------------------
// dummy functions because C++ can't handle pointers to member functions
// callback functions to the machine console.
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
// SynchConsole(char * in, char * out)
//     Constructor for the SynchConsole class.
//     in, out are parameters to the underlying console, they denote
//     the file descriptors to be used for reading and writing.
//----------------------------------------------------------------------

SynchConsole::SynchConsole(char * in, char * out){
  console = new(std::nothrow) Console(in, out, readAvailFunc, writeDoneFunc, (int) this);
  readAvail = new(std::nothrow) Semaphore("read available", 0);
  writeDone = new(std::nothrow) Semaphore("write done", 0);
  mutex = new(std::nothrow) Lock("synchconsole mutex");
}

//----------------------------------------------------------------------
// ~SynchConsole() 
//     Destructor for the SynchConsole class
//----------------------------------------------------------------------

SynchConsole::~SynchConsole() {
  delete console;
  delete readAvail;
  delete writeDone;
  delete mutex;

}

//----------------------------------------------------------------------
// ReadAvail()
//     called by the callback function from outside the class
//     increments the semaphore value to allow GetChar to proceed
//----------------------------------------------------------------------

void SynchConsole::ReadAvail() {
  readAvail->V();
}

//----------------------------------------------------------------------
// WriteDone()
//     called by the callback function from outside the class
//     increments the semaphore value to allow PutChar to proceed
//----------------------------------------------------------------------

void SynchConsole::WriteDone() {
  writeDone->V();
}

//----------------------------------------------------------------------
// WriteLine(char * line) 
//     line is a null-terminated char array that is to be output to 
//     the console.
//     acquires mutex on the console and then writes one char at a time.
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
// SynchPutChar(char ch)
//     ch is the char to be written to the console
//     Acquires mutex then writes the char to the console
//----------------------------------------------------------------------

void SynchConsole::SynchPutChar(char ch) {
  mutex->Acquire();
  PrivatePutChar(ch);
  mutex->Release();
}

//----------------------------------------------------------------------
// SynchGetChar()
//     returns the char input from the console
//     acqurires mutex on the console and then gets the char
//----------------------------------------------------------------------

char SynchConsole::SynchGetChar() {
  mutex->Acquire();

  char ch = PrivateGetChar();

  mutex->Release();
  return ch;
}

//----------------------------------------------------------------------
// PrivatePutChar(char ch)
//     ch is the char to be output to the console
//     private method that calls the actual console
//     and then decrements the appropriate semaphore to signal 
//     that the write is done.
//----------------------------------------------------------------------

void SynchConsole::PrivatePutChar(char ch) {
  console->PutChar(ch);
  writeDone->P();
}

//----------------------------------------------------------------------
// PrivateGetChar()
//     returns the char retrieved from the console
//     private method that calls the actual console
//     it decrements the appropriate semaphore to signal that no 
//     character is available
//----------------------------------------------------------------------

char SynchConsole::PrivateGetChar() {

  readAvail->P();		// wait for character to arrive
  char ch = console->GetChar();
  return ch;
}

#endif // CHANGED
