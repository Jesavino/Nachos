#ifdef CHANGED

#include "machine.h"
#include "addrspace.h"
//#include "system.h"

const static int maxPages = 20;

class MemoryManager {

	public:
		MemoryManager(Machine* sysMachine );
		bool ReadMem( int addr, int size, int *value);
		bool WriteMem( int addr, int size, int value);

		int Translate(int virtAddr, int* physAddr, int size, bool writing);

		int physPageInfo[NumPhysPages];		

	private:
		Machine* machine;
		TranslationEntry *tlb;



};


#endif
