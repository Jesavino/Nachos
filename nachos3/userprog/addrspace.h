// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#include "syscall.h"
#ifdef CHANGED
#include "memManager.h"
#include "synchdisk.h"
#endif

#define UserStackSize		2048 	// increase this as necessary!

#ifdef CHANGED
class MemoryManager;
class SynchDisk;

#endif

class PageInfo {
  public:
    int virtualPage;  	// The page number in virtual memory.
    int physicalPage;  	// The page number in real memory (relative to the
			//  start of "mainMemory"
		int diskPage;				// The disk segment the page lives on
    bool valid;         // If this bit is set, the translation is ignored.
			// (In other words, the entry hasn't been initialized.)
    bool readOnly;	// If this bit is set, the user program is not allowed
			// to modify the contents of the page.
    bool use;           // This bit is set by the hardware every time the
			// page is referenced or modified.
    bool dirty;         // This bit is set by the hardware every time the
			// page is modified.
		bool onDisk; 				// this is set when the page is moved into / out of
			// main memory.
};

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    AddrSpace(OpenFile *cp, int offset);

    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

#ifdef CHANGED
    PageInfo* getPageTable(); // in order to get access to the page table
    void PrintRegisters(); //just a function for debugging
    int getFail();
    int NumPages;
    int MaxVirtualAddress;
    PageInfo *pageTable;	// how we keep track of memory
    unsigned int numPages;		// Number of pages in the virtual 
															// address space		
    MemoryManager *memManager;
    int fail;
#endif
};

#endif // ADDRSPACE_H
