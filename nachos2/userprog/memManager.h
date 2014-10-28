#ifdef CHANGED
#ifndef __MEMMANAGER_H__
#define __MEMMANAGER_H__

#include "machine.h"
#include "addrspace.h"
//#include "system.h"

class AddrSpace;

const static int maxPages = 20;

class MemoryManager {

	public:
		MemoryManager(Machine* sysMachine );
		bool ReadMem( int addr, int size, int *value, AddrSpace * space);
		bool WriteMem( int addr, int size, int value, AddrSpace * space);

		int Translate(int virtAddr, int* physAddr, int size, bool writing, AddrSpace * Space);

		int physPageInfo[NumPhysPages];		

	private:
		Machine* machine;
		TranslationEntry *tlb;



};


#endif
#endif
