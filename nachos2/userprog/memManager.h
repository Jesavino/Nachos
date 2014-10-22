#ifdef CHANGED

#include "machine.h"


class MemoryManager {

	public:
		MemoryManager(Machine* sysMachine );
		bool ReadMem( int addr, int size, int *value);
		bool WriteMem( int addr, int size, int value);

		int Translate(int virtAddr, int* physAddr, int size, bool writing);



	private:
		Machine* machine;
		TranslationEntry *tlb;



};


#endif
