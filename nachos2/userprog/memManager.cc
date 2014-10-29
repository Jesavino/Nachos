#ifdef CHANGED

#include "memManager.h"

MemoryManager::MemoryManager(Machine *sysMachine ) {

	machine = sysMachine;
	for (int i = 0 ; i < NumPhysPages ; i++)
		physPageInfo[i] = 0;

}

bool
MemoryManager::ReadMem( int addr, int size, int *value, AddrSpace * space) {
	bool status;

	if (size < 1) {
		fprintf(stderr, "Cannot read less than one byte\n");
		return false;
	}
	for (int i = 0 ; i < size ; i++) {
		status = ReadOneByte(++addr, value, space);
		if (!status)
			return false;
	}
	return status;

}
bool
MemoryManager::ReadOneByte(int addr, int *value, AddrSpace *space) {
	int size = 1;
	int data, result;
	int physicalAddress;

	DEBUG('a', "Reading from VA 0x%x, size %d\n", addr, size);

	result = Translate(addr, &physicalAddress, size, false, space);
	if (result == -1)
		return false;

	data = machine->mainMemory[physicalAddress];
	*value = data;
	DEBUG('a', "\tvalue read = %8.8x\n", *value);
	return (true);

}
bool 
MemoryManager::WriteMem( int addr, int size, int *value, AddrSpace * space) {
	
	bool status;
	if ( size < 1 ) {
		fprintf(stderr, "Cannot write less than one byte\n");
		return false;
	}
	
	for ( int i = 0 ; i < size ; i++) {
		
		DEBUG('a', "Writing to VA 0x%x, size %d, value 0x%x\n", addr, size, value);
		status = WriteOneByte(addr, value[i], space); // Make sure to bump addr and value!
		addr++;
		value++;
		if(!status)
			return false;
	}
	return status;

}
bool
MemoryManager::WriteOneByte(int addr, int value, AddrSpace *space) {
	int size = 1;
	int physicalAddress;
	int result;
	DEBUG('a', "Writing to VA 0x%x, size %d, value 0x%x\n", addr, size, value);

	result = Translate(addr, &physicalAddress, size, true, space);

	if(result == -1)
		return false;
	fprintf(stderr , "Value is %d\n" , value);
	machine->mainMemory[physicalAddress] = (unsigned char) (value & 0xff);

	return true;

}

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
	
	/* THIS IS ALL CRAP FROM THE OLD TRANSLATE
	if( machine->tlb == NULL)
		fprintf(stderr, "Error, TLB is NULL!\n");
	else {
		for (entry = NULL, i = 0 ; i < TLBSize; i++)
			if (tlb[i].valid && ((unsigned)tlb[i].virtualPage == vpn)) {
				entry = &tlb[i];
				break;
			}
		if (entry == NULL) {
			// THIS IS WHAT WE NEED TO IMPLEMENT. YAY.
		}	
	}
	
	if (entry->readOnly && writing) {
		DEBUG('a', "%d mapped read-only at %d in TLB!\n", virtAddr, i);
		return -1;
	}	
	*/
	
	// Get the address space from the current thread
	pageTable = space->getPageTable();

	// we have the VPN, should line up with ith physical page it has
	if (pageTable == NULL) {
		fprintf(stderr, "Page Table null\n");
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
		fprintf(stderr, "Attempting to write a read only file!!!\n");
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
	fprintf(stderr, "PageFrame %d PageSize %d offSet %d\n", pageFrame, PageSize, offset);
	*physAddr = pageFrame * PageSize + offset;
	ASSERT((*physAddr >= 0) && ((*physAddr + size) <= MemorySize));
	DEBUG('a', "Phys addr = 0x%x\n", *physAddr);
	return 1;

}
#endif
