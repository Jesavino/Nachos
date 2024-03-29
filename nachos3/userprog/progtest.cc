// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include <new>

#ifdef CHANGED
#include "synchconsole.h"
#include "processinfo.h"

// globals
BitMap * bitmap;
BitMap * diskmap;
SynchDisk *disk;
#endif

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;
    bitmap = new(std::nothrow) BitMap(NumPhysPages);
    diskmap = new(std::nothrow) BitMap(NumSectors);
    char * diskname = "synchdisk";
    disk = new(std::nothrow) SynchDisk(diskname);
    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }
    space = new(std::nothrow) AddrSpace(executable);
#ifdef CHANGED
    //    if the address space did not constuct correctly
    if (space->fail) {
      return;
    }

    currentThread->space = space;
    // this process has no parent, but we will give it a parentid of -1
    currentThread->procInfo = new(std::nothrow) ProcessInfo(currentThread->pid, -1);
    //delete executable;			// close file
#endif

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.
#ifdef CHANGED
/*
static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int) { readAvail->V(); }
static void WriteDone(int) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new(std::nothrow) Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new(std::nothrow) Semaphore("read avail", 0);
    writeDone = new(std::nothrow) Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
*/

static SynchConsole * console;

void ConsoleTest(char * in, char * out) {
  char ch;
  console = new(std::nothrow) SynchConsole(in, out);
  while (1) {
    ch = console->SynchGetChar();
    console->SynchPutChar(ch);
    if (ch == 'q') return;

  }
}

#endif
