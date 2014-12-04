// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "addrspace.h"
#include "noff.h"
#include "system.h"
#include <new>

#ifdef CHANGED
extern BitMap * diskmap;
extern SynchDisk * disk;
// Returns available physical address
int
getDiskPageNum() {
	return diskmap->Find();
}
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//      If the address space creation has failed, it sets a flag, which is
//      in turn checked by the caller
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *file)
{
    NoffHeader noffH;
    unsigned int size;
#ifndef USE_TLB
    unsigned int i;
#endif
    executable = file;
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
	(WordToHost(noffH.noffMagic) == NOFFMAGIC))
      SwapHeader(&noffH);
#ifdef CHANGED
    offset = noffH.code.inFileAddr;
    // assume the creation has not failed yet
    fail = 0;
    if (noffH.noffMagic != NOFFMAGIC) {
      fail = 1;
      return;
    }
#endif
// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
      + UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;
    
#ifdef CHANGED
		// Set the maximum VA size possible for this addrspace
    MaxVirtualAddress = numPages * PageSize;


    // TODO
    // need to get rid of the assert and make sure that 
    // the system does not stop when we run a program that is too big.
    if ((unsigned int) diskmap->NumClear() < numPages) {
      numPages = 0;
      fail = 1;
      return;
    }
    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
#endif
    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
#ifdef CHANGED

    //NumPages = numPages; // set global access for TLB management
    //memManager = new(std::nothrow) MemoryManager(machine);
    memManager = new(std::nothrow) MemoryManager(machine);
    
//#ifndef USE_TLB
// first, set up the translation 
    int physPage;
    // int zeros[PageSize] = {0};
    pageTable = new(std::nothrow) PageInfo[numPages];
    for (unsigned int i = 0; i < numPages; i++) {
      pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
      physPage = -1;
      pageTable[i].physicalPage = physPage;
      pageTable[i].diskPage = -1;
      pageTable[i].valid = true;
      pageTable[i].use = false;
      pageTable[i].dirty = false;
      pageTable[i].readOnly = false;  // if the code segment was entirely on 
      // a separate page, we could set its 		
      // pages to be read-only

      //      memManager->Translate(pageTable[i].virtualPage * PageSize, &physAddr, 1, true, this);
      //bzero(machine->mainMemory + physAddr, PageSize);

    }
    //    fprintf(stderr, "\n******** PAST THE ALLOCATION *********\n");
    
    
    //fprintf(stderr, "Writing %d bytes to VA 0x%x\n", noffH.code.size, noffH.code.virtualAddr);
#endif
    
}

#ifdef CHANGED
//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//     Alternate constructor for loading from a checkpoint.
//     cp is the file that has been checkpointed.
//     headersize is the amount of data before the pages in the file
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *cp, int headerSize){
  executable = cp;
  offset = headerSize;
  memManager = new(std::nothrow) MemoryManager(machine);

  MaxVirtualAddress = executable->Length() - offset;
  fail = 0;
  int physPage;
  numPages = divRoundUp(MaxVirtualAddress, PageSize);

  pageTable = new(std::nothrow) PageInfo[numPages];
  for (unsigned int i = 0; i < numPages; i++) {
    pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
    physPage = -1;
    pageTable[i].physicalPage = physPage;
    pageTable[i].diskPage = -1;
    pageTable[i].valid = true;
    pageTable[i].use = false;
    pageTable[i].dirty = false;
    pageTable[i].readOnly = false;  // if the code segment was entirely on 
      // a separate page, we could set its 		
      // pages to be read-only
  }
}

#endif
//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
#ifdef CHANGED
  if (!fail) {
    // for each page on disk, mark the bitmap as clear, so the space can be reused
    for (unsigned int i = 0; i < numPages; i++){
      //    printf("%d\n", pageTable[i].diskPage);
      if (pageTable[i].diskPage != -1)
	diskmap->Clear(pageTable[i].diskPage);
    }
    
    //#endif
    //#ifndef USE_TLB
    delete [] pageTable;
    // need to make sure the pageTable pointer is null for checks in system calls
    pageTable = NULL;
    delete memManager;
  }
  // because the executable is stored in addrspace now for lazy loading.
  delete executable;
}


int AddrSpace::getFail() {
  return fail;
}

//----------------------------------------------------------------------
// AddrSpace::LoadPageToDisk
//     loads a page from an executable to the disk. abstracted for
//     lazy loading. 
//     vpn is the virtual page number to be loaded.
//----------------------------------------------------------------------
void AddrSpace::LoadPageToDisk(int vpn){

  char buffer[PageSize];
  // j < size of executable or max virtual address
  pageTable[vpn].diskPage = getDiskPageNum();
  executable->ReadAt(buffer, PageSize, vpn * PageSize + offset);
  disk->WriteSector(pageTable[vpn].diskPage, buffer);
  
}
#endif

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	
    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);
   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------

void
AddrSpace::PrintRegisters() {
  //int i;
	fprintf(stderr, "PCReg:   %d NextPCReg:    %d StackReg: %d\n", machine->ReadRegister(PCReg),
		machine->ReadRegister(NextPCReg), machine->ReadRegister(StackReg));

}
//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{
#ifdef CHANGED
  // invalidate the tlb on context switches
  for (int i = 0; i < 4; i++) {
    machine->tlb[i].valid=0;
  }
#endif
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table,
//      IF address translation is done with a page table instead
//      of a hardware TLB.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifndef USE_TLB
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
#endif

}


#ifdef CHANGED

//----------------------------------------------------------------------
// AddrSpace::getPageTable()
//     returns the address of the pageTable associated with this instance
//     of the address space
//----------------------------------------------------------------------

PageInfo *AddrSpace::getPageTable() {
	return pageTable;
}
#endif
