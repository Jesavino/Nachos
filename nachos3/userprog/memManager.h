#ifdef CHANGED
#ifndef __MEMMANAGER_H__
#define __MEMMANAGER_H__

#include "machine.h"
#include "addrspace.h"

// A class for managing memory. All VA to PHYS address translation goes through
// the memory manager
class AddrSpace;
class PageInfo;

class MemoryManager {

	public:
		// pass in a machine to access mainMemory
		MemoryManager(Machine* sysMachine );

		// Read size bytes from VA addr into value. The AddrSpace is needed to get 
		// the correct page table 
		bool ReadMem( int addr, int size, int *value, AddrSpace * space);

		// Write size bytes from value into VA addr. 
		bool WriteMem( int addr, int size, int value, AddrSpace * space);

		// Translate VA virtAddr and put it into physAddr
		int Translate(int virtAddr, int* physAddr, int size, bool writing, AddrSpace * Space);

		// An array that makes note of which physical pages a process has access to
		int physPageInfo[NumPhysPages];		

	private:
		Machine* machine;
		PageInfo *tlb;



};


#endif
#endif
