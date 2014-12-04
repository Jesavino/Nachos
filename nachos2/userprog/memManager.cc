#ifdef CHANGED

#include "memManager.h"
// -----------------------------------------------------------------------
//
// Constructor. Sets up the memory manager to reference the correct 
// batch of mainMemory. 
//
// ----------------------------------------------------------------------
MemoryManager::MemoryManager(Machine *sysMachine ) {

	machine = sysMachine;
	for (int i = 0 ; i < NumPhysPages ; i++)
		physPageInfo[i] = 0;

}
// -------------------------------------------------------------------------
//
// Reads memory at virtual Addr addr. 
// Returns: True if the memory was read correctly
// 					False if an error occured
//
// 	-----------------------------------------------------------------------
bool
MemoryManager::ReadMem( int addr, int size, int *value, AddrSpace * space) {
	int data, result;
	int physicalAddress;
	DEBUG('a', "Reading from VA 0x%x, size %d\n", addr, size);

	result = Translate(addr, &physicalAddress, size, false, space);
	if (result == -1)
		return false;
	switch (size) {
		case 1:
			data = machine->mainMemory[physicalAddress];
			*value = data;
			break;
		
		case 2:
			data = *(unsigned short *) &machine->mainMemory[physicalAddress];
			*value = ShortToHost(data);
			break;

		case 4:
			data = *(unsigned int *) &machine->mainMemory[physicalAddress];
			*value = WordToHost(data);
			break;

		default: ASSERT(false);
	}
	DEBUG('a', "\tvalue read = %8.8x\n", *value);
	return (true);

}
// ---------------------------------------------------------------------------
//
// Writes size bytes to VA addr fomr value. 
// Returns: True if the write was successful
// 					False if an error occured
//
// ---------------------------------------------------------------------------
bool 
MemoryManager::WriteMem( int addr, int size, int value, AddrSpace * space) {
	int physicalAddress;
	int result;
	DEBUG('a', "Writing to VA 0x%x, size %d, value 0x%x\n", addr, size, value);

	result = Translate(addr, &physicalAddress, size, true, space);
	if(result == -1)
		return false;
	switch (size) {
		case 1:
			machine->mainMemory[physicalAddress] = (unsigned char) (value & 0xff);
			break;
		
		case 2:
			*(unsigned short *) &machine->mainMemory[physicalAddress]
				= ShortToMachine((unsigned short) (value & 0xffff));
			break;

		case 4:
			*(unsigned int *) &machine->mainMemory[physicalAddress]
				= ShortToMachine((unsigned short) (value & 0xffff));
			break;

		default: ASSERT(false);
	}

	return true;

}
// ------------------------------------------------------------------------------
//
// Tranlates VA virtAddr to the correct physical address. 
// uses a pages reference map from the AddrSpace to find the correct physical 
// page
// Returns: -1 if an error occured
// 					1 otherwise
//
// -------------------------------------------------------------------------------
int 
MemoryManager::Translate(int virtAddr, int* physAddr, int size, bool writing, AddrSpace * space) {
	unsigned int i;
	unsigned int vpn, offset;
	TranslationEntry *entry;
	unsigned int pageFrame;
	TranslationEntry *pageTable;
	DEBUG('a', "\tTranslate 0x%x, %s: ", virtAddr, writing ? "write" : "Read");

	// check for alignment errors
	if (((size == 4) && (virtAddr & 0x3)) || ((size == 2) && (virtAddr & 0x1))){
		DEBUG('a', "Alignment problem at %d, size %d!\n", virtAddr, size);
		return -1;
	}
	// calculate the vpn and offset within the page from VA
	vpn = (unsigned) virtAddr / PageSize;
	offset = (unsigned) virtAddr % PageSize;
		
	// Get the address space from the current thread
	pageTable = space->getPageTable();

	// we have the VPN, should line up with ith physical page it has
	if (pageTable == NULL) {
		//fprintf(stderr, "Page Table null\n");
		return -1;
	}
	for (entry = NULL, i = 0 ; i < space->numPages ; i++) {
		if (pageTable[i].valid && ((unsigned)pageTable[i].virtualPage == vpn)) {
			entry = &pageTable[i];
			break;
		}
	}
	if ( entry == NULL)
		return -1;
	if (entry->readOnly && writing) {
		//fprintf(stderr, "Attempting to write a read only file!!!\n");
		return -1;
	}
	pageFrame = entry->physicalPage;

	// if the pageFrame is too big, there is a problem. Something wrong 
	// loaded from TLB
	if (pageFrame >= NumPhysPages) {
		DEBUG('a', "*** frame %d > %d!\n", pageFrame, NumPhysPages);
		return -1;
	}
	entry->use = false;
	if (writing)
		entry->dirty = true;
	*physAddr = pageFrame * PageSize + offset;
	ASSERT((*physAddr >= 0) && ((*physAddr + size) <= MemorySize));
	DEBUG('a', "Phys addr = 0x%x\n", *physAddr);
	return 1;

}
#endif
