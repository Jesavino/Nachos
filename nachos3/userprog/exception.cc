// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchconsole.h"
#ifdef CHANGED
#include "thread.h"
#include "processinfo.h"
#endif
#ifdef USE_TLB

#ifdef CHANGED
#define CPNUMBER 0xfafb
//----------------------------------------------------------------------
// Space for Global Data as needed
//
//----------------------------------------------------------------------

// Specify the max number of files that can be opened
static const int NumOpenFiles = 20;

// Global Process ID
int pid = 1;

// Globals to be set if there are arguments to exec 
// The size of Args can change if more than 10 args will be passed
// argSpace is access to the newly created addrSpace for arguments
int argc;
char *args[10];
AddrSpace *argSpace; // new space for args

// This struct manages info about open files. Each file is indexed 
// in an array so we can access them at any time
struct openFileDesc {
	OpenFile *openFile = NULL;
	int used;
	int refCount;
	char * name;
}openFiles[NumOpenFiles];

// Some address and string globals really only used per syscall
char * stringarg;
int whence;

// for page replacement
int commutator = 0;

//global struct for storing physical page information needed.
struct PhysPageDesc{
  int vpn;
  int diskpage;
  bool valid;
  bool dirty;
  // it has a pointer to the address space of the owner of the page
  AddrSpace * space;
  // pages are locked with a bool instead of an actual lock, 
  // because Locks were giving concurrency issues.
  Lock *pageLock = new(std::nothrow) Lock("pageLock");
  //  bool pageLock;
}physPageDesc[NumPhysPages];
// The SynchConsole acts as a wrapper around the Console Singleton
SynchConsole *synchConsole;

extern SynchDisk * disk;

// Lock to ensure mutex throughout access to process data
Lock * procLock = new(std::nothrow) Lock("global process lock");

// a space to store all the registers for the checkpointing process
int regs[NumTotalRegs];

//debug method
void printppd() {
  for (int i = 0; i < NumPhysPages; i++) {
    printf("physical Page: %d diskPage: %d vpn: %d\n", i, physPageDesc[i].diskpage, physPageDesc[i].vpn);
  }
}

// declaration
void LoadPageToMemory(int vpn, AddrSpace *space);

//-----------------------------------------------------------------------------
// WriteDirtyPage
//     writes the physPage to its correct spot on disk
//-----------------------------------------------------------------------------

void WriteDirtyPage(int physPage) {

  if (physPageDesc[physPage].dirty) {
    int physAddr = physPage * PageSize;
    int diskSector = physPageDesc[physPage].diskpage;
    char * buffer = new char[128];
    for (int i = 0; i < PageSize; i++) {
      buffer[i] = machine->mainMemory[physAddr + i];
    }
    disk->WriteSector(diskSector, buffer);
    //write page back to disk
    delete [] buffer;
  }
  // once the page is written to disk, it is no longer dirty.
  physPageDesc[physPage].dirty = false;

}

//-----------------------------------------------------------------------------
// LockPage
//     Abstraction to lock the physical page
//     ensures no other process can use it
//     uses vpn to lock, and consequently, the LockPage method
//     can call LoadPageToMemory if the page is not in physical memory
//-----------------------------------------------------------------------------
void
LockPage(int vpn) {
  //get information required to determine the physical page
	int virtualPage = vpn / PageSize;
	AddrSpace *space = currentThread->space;
	PageInfo *table = space->getPageTable();
	/*
	int physPage = table[virtualPage].physicalPage;
	if ( physPage == -1) {
	 	LoadPageToMemory(virtualPage , space);
		physPage = table[virtualPage].physicalPage;
	}

	// and the page is then locked, and set to valid.
	//physPageDesc[physPage].pageLock = true;
	physPageDesc[physPage].pageLock->Acquire();
	*/
	int physPage = -1;
	while (physPage == -1) {
		physPage = table[virtualPage].physicalPage;

		if (physPage == -1) {
			LoadPageToMemory(virtualPage, space);
			physPage = table[virtualPage].physicalPage;
		}
		physPageDesc[physPage].pageLock->Acquire();
		if (table[virtualPage].physicalPage == -1)
		  physPageDesc[physPage].pageLock->Release();

		physPage = table[virtualPage].physicalPage;

	}

	physPageDesc[physPage].valid = true;
}

//-----------------------------------------------------------------------------
// ReleasePage
//     Removes the lock from the physical page.
//-----------------------------------------------------------------------------
void 
ReleasePage(int vpn) {
  // get information required to determine the physical page
	int virtualPage = vpn / PageSize;
	AddrSpace *space = currentThread->space;
	PageInfo *table = space->getPageTable();
	int physPage = table[virtualPage].physicalPage;
	// and the lock is released.
	//	physPageDesc[physPage].pageLock = false;
	physPageDesc[physPage].pageLock->Release();
}

//-----------------------------------------------------------------------------
// FindPageToReplace
//     Uses the Fancy clock algorithm to find a physical page to replace
//-----------------------------------------------------------------------------
int FindPageToReplace() {
  for (int i = 0; i < NumPhysPages; i++) {
    
    // find an unlocked page that is not valid or dirty.
    if (!physPageDesc[commutator].valid && !physPageDesc[commutator].dirty) {
	//	&& !physPageDesc[commutator].pageLock) {
      int val = commutator;
      //increment the commutator before returning
      commutator = (commutator +1) % NumPhysPages;
      return val;
    }
    // increment the commutator 
    commutator = (commutator +1) % NumPhysPages;
  }

  // if there were no invalid pages, we need to go through again and find a page 
  // that is not valid and is dirty. 
  // in this loop, we also set each page we come across to not valid
  for (int i = 0; i < NumPhysPages; i++) {
    if (!physPageDesc[commutator].valid && physPageDesc[commutator].dirty) {
      //	&& !physPageDesc[commutator].pageLock) {
      //return dirty page
      int val = commutator;
      //increment the commutator befor returning
      commutator = (commutator +1) % NumPhysPages;
      return val;
    }
    // set the page to not valid and increment the commutator
    physPageDesc[commutator].valid = false;
    commutator = (commutator + 1) % NumPhysPages;
  }
  // 1 recursive call is made. According to fancy clock specs, it should return a
  // value because there is guaranteed to be an invalid page.
  return FindPageToReplace();
}

//-----------------------------------------------------------------------------
// LoadPageToMemory
//     Loads a page from the disk to physical memory.
//-----------------------------------------------------------------------------

void LoadPageToMemory(int vpn, AddrSpace * space) {

  //if page in memory, return
  if (space->pageTable[vpn].physicalPage != -1) return;
  
  // if the page is not on disk (lazy loading) load it to disk
  if (space->pageTable[vpn].diskPage == -1) space->LoadPageToDisk(vpn);

  //increase the number of pageFaults if the page is not in physical memory.
  stats->numPageFaults++;
  int pageToReplace = FindPageToReplace();
  //  physPageDesc[pageToReplace].pageLock = true;
  //  LockPage(vpn);
  physPageDesc[pageToReplace].pageLock->Acquire();
  for (int i = 0; i < TLBSize; i++) {
    if (machine->tlb[i].physicalPage == pageToReplace) {
      // invalidate the tlb entry for this page, if it is in the tlb.
      machine->tlb[i].valid = false;
    }
  }
  int physAddr = pageToReplace * PageSize;

  WriteDirtyPage(pageToReplace);

  // we need to make sure that the page we are replace is properly marked as no longer in
  // the owning processes pageTable
  // if there is no associated pageTable, that process is no longer running.
  if (physPageDesc[pageToReplace].space != NULL) {
    AddrSpace * otherSpace = physPageDesc[pageToReplace].space;
    if (otherSpace->pageTable != NULL)
      otherSpace->pageTable[physPageDesc[pageToReplace].vpn].physicalPage = -1;
  }
  space->pageTable[vpn].physicalPage = pageToReplace;

  //read page from disk into mainmemory[physAddr]
  char *buffer = new char[128];
  disk->ReadSector(space->pageTable[vpn].diskPage, buffer);
  
  // write the sector to physical memory
  for (int i = 0; i <PageSize; i++) {
    machine->mainMemory[physAddr + i] = buffer[i];
  }

  delete [] buffer;

  // set the information in the physPageDesc struct
  physPageDesc[pageToReplace].vpn = vpn;
  physPageDesc[pageToReplace].diskpage = space->pageTable[vpn].diskPage;
  physPageDesc[pageToReplace].valid = true;
  physPageDesc[pageToReplace].dirty = true;
  physPageDesc[pageToReplace].space = space;
  physPageDesc[pageToReplace].pageLock->Release();
}

//-----------------------------------------------------------------------------
// Increments the program counters
//-----------------------------------------------------------------------------
void incrementPC() {

	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, pc);
	pc += 4;
	machine->WriteRegister(NextPCReg, pc);
}

//-----------------------------------------------------------------------------
// 
// Called via the Create() Syscall. Assumes the filename is the only argument
// Will print out info if the creation failed, but otherwise no return value 
// is specified.
// LIMIT: Filenames can be at most 127 characters plus the null terminator
//
// ----------------------------------------------------------------------------
void createNewFile() {
	int physAddr;
	DEBUG('a', "Creating a new file.\n");
	stringarg = new (std::nothrow) char[128];
	whence = machine->ReadRegister(4);
	AddrSpace *space = currentThread->space;
	// Translate loop to get the file name. This is done byte by byte
	for ( int i = 0 ; i < 127 ; i++) {
		LockPage(whence);
		if( ! space->memManager->Translate(whence, &physAddr, 1, false, space) ) {
			//fprintf(stderr, "Bad Translation\n");
		  ReleasePage(whence);
			break;
		}
		ReleasePage(whence);
		if ((stringarg[i] = machine->mainMemory[physAddr]) == '\0') break;		 
		
		whence++;
	}
	stringarg[127] = '\0';
	
	if ( ! fileSystem->Create(stringarg, 0) ) {// second arg not needed, dynamic file size
	  delete [] stringarg;
		return;
	}
	  //fprintf(stderr, "File Creation Failed. Either the file exists or there are memory problems\n");
	delete [] stringarg;	
}
//-------------------------------------------------------------------------------------------
//
// Called as a result of the Open() syscall. When the file is opened, the handler finds an 
// empty slot in the array of open file storage. In addition, it updates the bitmap of the 
// current process to indicate which files the currentThread has open. This is utilizaed only 
// for file sharing between exec'd processes
// LIMIT: Again, file names can only be 127 chars plus the null byte
// Returns: -1 on error
// 					An OpenFileId otherwise
//
// -------------------------------------------------------------------------------------------					
void openFile() {
	int physAddr;
	AddrSpace *space = currentThread->space;
	DEBUG('a', "Opening file.\n");
	stringarg = new(std::nothrow) char[128];
	whence = machine->ReadRegister(4);
	// Translate byte by byte to get the filename to open
	for ( int i = 0 ; i < 127 ; i++ ) {
		LockPage(whence);
		if ( !space->memManager->Translate(whence, &physAddr, 1, false, space) ) {
			//fprintf(stderr, "Bad Translation\n");
		  ReleasePage(whence);
			break;
		}
		ReleasePage(whence);
		if ((stringarg[i] = machine->mainMemory[physAddr]) == '\0') break;
		whence++;
	}
	stringarg[127] = '\0';

	// Open the file and make sure it opened properly
	OpenFile *file = fileSystem->Open(stringarg);

	if (file == NULL){
		//fprintf(stderr, "Error during file opening\n");
		machine->WriteRegister(2, -1);
		delete [] stringarg;
		return;
	}
	// init the bitmap if it has not been done already
	if (currentThread->openFilesMap == NULL)
		currentThread->openFilesMap = new(std::nothrow) BitMap(NumOpenFiles);

	// Find a place for the file
	int fileId = -1;
	for ( int i = 2 ; i < NumOpenFiles ; i++) {
		if (! openFiles[i].used ) {
			openFiles[i].used = 1;
			openFiles[i].refCount++;
			openFiles[i].name = stringarg;
			openFiles[i].openFile = file;
			fileId = i;
			currentThread->openFilesMap->Mark(i); //mark the file as opened by this parent
			break;
		}
	}
	if (fileId == -1) {
		//fprintf(stderr, "Too many files open\n");
		machine->WriteRegister(2,-1);
		delete [] stringarg;
		return;
	}
	else
		machine->WriteRegister(2, fileId);
	delete [] stringarg;
}
//-----------------------------------------------------------------------------------------
//
// Writes a specified number of bytes to an already open file. If the file specified is
// ConsoleOutput, the number of bytes is instead written to the console
// LIMIT: Same file limitations as open and create
// Returns: -1 on error
// 					The number of bytes written otherwise
//
// ----------------------------------------------------------------------------------------					
void writeFile() {
	int physAddr;
	AddrSpace *space = currentThread->space;
	DEBUG('a', "Writing to a file\n");
	
	int size = machine->ReadRegister(5);
	whence = machine->ReadRegister(4);
	stringarg = new(std::nothrow) char[size + 1];

	// Get the string to be written from userland
	for (int i = 0 ; i < size ; i++) {
	  LockPage(whence + i);
	  if ( ! space->memManager->Translate(whence + i, &physAddr, 1, false, space)) {
	    //fprintf(stderr, "Error in Write Translation\n");
	    ReleasePage(whence + i);
	    break;
	  }
	  if ((stringarg[i] = machine->mainMemory[physAddr]) == '\0') {
	    ReleasePage(whence + i);
	    break;
	  }
	  else ReleasePage(whence + i);
	}
	stringarg[size] = '\0';

	int file = machine->ReadRegister(6);
	// If the file specified is the console
	if ( file == ConsoleOutput ) {
		if (synchConsole == NULL) synchConsole = new(std::nothrow) SynchConsole(NULL, NULL);
		synchConsole->WriteLine(stringarg);
		machine->WriteRegister(2, size);
	}
	else if ( file == ConsoleInput ){
		//fprintf(stderr, "Cannot Write to StdInput\n");
		machine->WriteRegister(2, -1);
	}
	else if (! currentThread->openFilesMap->Test(file) )
		machine->WriteRegister(2, -1);
	else {
		if( !openFiles[file].used ){
			//fprintf(stderr, "Requested file has not been opened!\n");
			machine->WriteRegister(2, -1);
			//			delete [] stringarg;
			return;
		}
		OpenFile *fileToWrite = openFiles[file].openFile;
		int numWrite = fileToWrite->Write( stringarg, size);
		machine->WriteRegister(2, numWrite);
	}	
	//	delete [] stringarg;
}
//-----------------------------------------------------------------------------------------
//
// Reads a specified number of bytes from a specified file. If ConsoleInput is specified, 
// it attempts to read from there. 
// LIMIT: Can only read at most 128 bytes from the console at a time
// Returns: -1 if an error occured
// 					the number of bytes read otherwise
//
// -----------------------------------------------------------------------------------------
void readFile() {
	DEBUG('a' , "Reading from File\n");
	AddrSpace* space = currentThread->space;
	int file = machine->ReadRegister(6);
	// make sure we are reading a file that COULD exist
	if (file < 0 || file > NumOpenFiles) {
		//fprintf(stderr, "Invalid file read attempt\n");
		machine->WriteRegister(2, -1);
		return;
	}
	int numBytes = machine->ReadRegister(5);
	whence =  machine->ReadRegister(4);	
	if (file == ConsoleInput) {
		char * readChar = new(std::nothrow) char[128];
		if (synchConsole == NULL) synchConsole = new(std::nothrow) SynchConsole(NULL, NULL);
		for( int i = 0 ; i < numBytes ; i++) {
		  readChar[i] = synchConsole->SynchGetChar();
		  if ( ! space->memManager->WriteMem(whence + i , 1, (int) readChar[i], space)) {
			  machine->WriteRegister(2, -1);
			  delete [] readChar;
			  return;
		  }
		} 	
		machine->WriteRegister(2, numBytes);
		delete [] readChar;
	}
	else if(!openFiles[file].used){
		// error reading from closed or non-existing file
		machine->WriteRegister(2, -1);
	}
	else if( currentThread->openFilesMap == NULL || !currentThread->openFilesMap->Test(file)) {
		machine->WriteRegister(2, -1);
	}
	else {
		char * buffer = (char *) malloc( sizeof(char*) * numBytes );
		OpenFile *fileToRead = openFiles[file].openFile;
		int numRead = fileToRead->Read( buffer , numBytes );

		for ( int i = 0 ; i < numBytes ; i++) {
			LockPage(whence + i);
			if ( !space->memManager->WriteMem(whence + i , 1, (int) buffer[i],space)) {
			  machine->WriteRegister(2,-1);
				ReleasePage(whence + i);
				return;
			}
			ReleasePage(whence + i);
		}
		machine->WriteRegister(2 , numRead);
	}
}
// ------------------------------------------------------------------------------------
//
// Closes the specified file. Checks to make sure that nobody else is using the same 
// file before it is close. By convetion, if a parent execs a kid with shared file 
// access, the kid MUST close the file(s) before exiting.
// Limit: none
// Returns: nothing
//
// -------------------------------------------------------------------------------------
void closeFile() {
	DEBUG('a', "Closing the file\n");

	int file = machine->ReadRegister(4);

	if (file == ConsoleInput)
		return;
	  //fprintf(stderr, "Cannot close Console Input\n");
	else if (file == ConsoleOutput)
		return;
	  //fprintf(stderr, "Cannot close Console Output\n");
	else {
		if (!openFiles[file].used)
			return;
		  		//fprintf(stderr, "File not open to be closed!\n");
		if ( currentThread->openFilesMap == NULL || !currentThread->openFilesMap->Test(file)) {
			return;
			//fprintf(stderr, "Cannot Close a file you do not own\n");
		}
		// only delete the file from the list of open files if you are the last one using it
		openFiles[file].refCount--;
		if ( (openFiles[file].refCount) == 0 ) {
			openFiles[file].used = 0;
			openFiles[file].name = NULL;
			openFiles[file].openFile = NULL;
		}
		currentThread->openFilesMap->Clear(file);
	}
}

/*void forkProgram() {
  printf("in fork\n");
  VoidFunctionPtr func;
  func = (VoidFunctionPtr)machine->ReadRegister(4);

  Thread * t = new(std::nothrow) Thread("userprog");
  t->Fork(func, 0);
}

void yieldProgram() {
  printf("in yield\n");
  currentThread->Yield();
}
*/

// --------------------------------------------------------------------------------
//
// Preps the stack with user arguments. This is only called if there are arguments 
// with which to prep the stack
// 
// -------------------------------------------------------------------------------
void prepStack(int argcount, char **argv, AddrSpace *space) {
	int sp;
	int len;
	int argvAddr[argcount];
	int physAddr;
	char * tmp;
	sp = machine->ReadRegister(StackReg);
	for ( int i = 0 ; i < argcount ; i++) {
		len = strlen(argv[i]) + 1;
		sp -= len;
		tmp = argv[i];
		//fprintf(stderr, "tmp is %s\n", tmp);
		for ( int j = 0; j < len ; j++) {
			LockPage(sp + j);
			space->memManager->Translate(sp + j, &physAddr, 1, false, space);
			machine->mainMemory[physAddr] = tmp[j];
			physPageDesc[physAddr/PageSize].dirty = 1;
			ReleasePage(sp +j);
		}
		argvAddr[i] = sp;

	}
	// align SP
	sp = sp & ~3;

	// Allocate and fill the argv array
	sp -= sizeof(int) * argcount;

	for ( int i = 0 ; i < argcount; i++ ) {
		LockPage(sp + i*4);
		space->memManager->Translate(sp + i*4, &physAddr, 4, false, space);
		*(unsigned int *) &machine->mainMemory[physAddr] = WordToMachine((unsigned int) argvAddr[i]);
		physPageDesc[physAddr/PageSize].dirty = 1;
		ReleasePage(sp + i*4);
	}
	// Fill reg 4 with number of arguments and reg 5 with address of first argument
	machine->WriteRegister(4, argc);
	machine->WriteRegister(5, sp);
	machine->WriteRegister(StackReg, sp - 8);
	
}

// ----------------------------------------------------------------------
// execThread
//    Called to prepare the registers and stack after a thread has been 
//    created. This eventually called machine->Run() on the exec'd thread
//
// ---------------------------------------------------------------------

void execThread(int arg) {
  // registers are properly initialized
  currentThread->space->InitRegisters();

  // if this was a checkpointed program, we restore the registers from the 
  // global regs array
  // all the registers except NextPCReg are initialized to 0 (NextPCReg ==4)
  for (int i = 0; i < NumTotalRegs; i++) {
    if (regs[i] != 0) {
      machine->WriteRegister(i, regs[i]);
    }
  }

  // if its a checkpointed file, PCReg is not going to be 0
  // so we need to increment the ProgramCounter to get to the next step, 
  // which is what would have happened directly after the initial checkpoint
  if (regs[PCReg] != 0){
    incrementPC();
  }

  currentThread->space->RestoreState();		// load page table register

  // If there were args, prep the stack here
  if(argc) 
    prepStack(argc, args, argSpace);
  
  machine->Run();			// jump to the user progam
  ASSERT(false);			// machine->Run never returns;
                                        // the address space exits
                                        // by doing the syscall "exit"

}

// -------------------------------------------------------------------------
// 
// Called as a result of the Exec() syscall. Functions expecting three 
// arguments, the name of the binary to exec to, an array of arguments to 
// send to the child, and a flag to determine if files should be shared 
// between parent and child. 
// Returns: -1 on error
// 					PID of kid otherwise
//
// -------------------------------------------------------------------------

void execFile() {
  argc = 0; // set number of args to 0 until we check
  char * filename = new(std::nothrow) char[128];
  whence = machine->ReadRegister(4);
  // get char * address from information at argv, then argv+1 etc.
  AddrSpace *newSpace;
  AddrSpace *space = currentThread->space;
  int physAddr;
	// Address translation
  for ( int i = 0 ; i < 127 ; i++) {
    LockPage(whence + i);
    
    if  ( !space->memManager->Translate(whence + i, &physAddr, 1, false, space)) {
      machine->WriteRegister(2,-1);
      ReleasePage(whence + i);
      return;
    }
    if ((filename[i] = machine->mainMemory[physAddr]) == '\0') {
      ReleasePage(whence + i);
      break;
    }
    ReleasePage(whence + i);
  }
  filename[127] = '\0';


  // Get arguments from the kernel
  char * argv[10] = {NULL};
  char * tmp = new(std::nothrow) char[128];
  int arg;
  whence = machine->ReadRegister(5);
  LockPage(whence);
  space->memManager->ReadMem(whence, 4, &arg, space);
  ReleasePage(whence);
  int j = 0;
  int size;
  while(arg != 0 && whence !=0)  {
    LockPage(arg);
    space->memManager->Translate(arg, &physAddr, 1, false, space);
    for ( int i = 0 ; i < 127 ; i++) {
      if((tmp[i] = machine->mainMemory[physAddr+i]) == '\0'){
	break;
      }
    }
    tmp[127] = '\0';
    size = sizeof(char) * (strlen(tmp)+1); // null terminator
    argv[j] = new(std::nothrow) char[size];
    strcpy(*(argv + j) , tmp);		
    j++;
    ReleasePage(arg);		
    whence += 4;
    LockPage(whence);
    space->memManager->ReadMem(whence, 4, &arg, space);
    ReleasePage(whence);
  }
  for (int i  = 0 ; i < 10 ; i++) {
    if( argv[i] == NULL) break;
    args[i] = argv[i];
  }
  // set global data for prep
  argc = j;
  delete [] tmp;
  OpenFile *executable = fileSystem->Open(filename);
  if (executable == NULL) {
    machine->WriteRegister(2, -1);
    return;
  }


  //CHECK IF file is reinstantiation of checkpoint using CPNUMBER
  // initialize the register array, because these will be written to 
  // the registers if this is a checkpoint reinstantiation or not
  for (int i = 0; i < NumTotalRegs; i ++) {
    regs[i] = 0;
  }
  regs[NextPCReg] = 4;
  
  // check the magic number at the head of the file
  int cpnum;
  executable->ReadAt((char*)&cpnum, sizeof(int), 0);
  if (cpnum == CPNUMBER) {
    // and if the number matches, then we load the rest of the information in the header
    // which amounts to 40 registers
    argc = 0;
    j = 4;
    for (int i = 0; i < NumTotalRegs; i++) {
      executable->ReadAt((char *) &regs[i], sizeof(int), j);
      j+=4;
    }
    newSpace = new(std::nothrow) AddrSpace(executable, NumTotalRegs * sizeof(int) + 4);
  }

  // create a new addressSpace to be given to new thread
  else {
    newSpace = new(std::nothrow) AddrSpace(executable);
    regs[NextPCReg] = 4;
  }

  if (newSpace->getFail()) {
    // if address space failed for any reason, delete it
    // and return -1
    delete newSpace;
    machine->WriteRegister(2, -1);
    return;
  }

  Thread * thread = new(std::nothrow) Thread("execed thread");
  thread->space = newSpace;
  
  // each new execed program is given a pid that is in sequence.
  // We have not worried about overflow
  thread->pid = pid++;
  // don't need mutex on processinfo here, because a child does not exist yet to
  // modify it.
  thread->procInfo = new(std::nothrow) ProcessInfo(thread->pid, currentThread->pid);
  currentThread->procInfo->AddChild(thread->procInfo);

  // if files are to be shared, make sure the new threads bitmap matches that of 
  // the old thread
  if (machine->ReadRegister(6)) {
    BitMap *bitmapCopy = new(std::nothrow) BitMap(NumOpenFiles);
    for (int i = 0 ; i < NumOpenFiles ; i++) {
      if(currentThread->openFilesMap->Test(i)) {
	bitmapCopy->Mark(i);
	openFiles[i].refCount++;
      }
    }
    thread->openFilesMap = bitmapCopy; // share all open files
  }
  
  // calling thread given this threads pid
  machine->WriteRegister(2, thread->pid);
  argSpace = thread->space;

  thread->Fork(execThread, 0);
	
  //  delete executable;			// close file
}  
// ----------------------------------------------------------------------
//
// Closes / decrements the reference count of files. This is called
// solely by exit in order to terminate all open files if the user
// did not do so
//
// ----------------------------------------------------------------------
void
exitCloseFile(int i) {
	
	if (i == ConsoleInput)
		return;
	  //fprintf(stderr, "Cannot close Console Input\n");
	else if (i == ConsoleOutput)
		return;
	  //fprintf(stderr, "Cannot close Console Output\n");
	else {
		if (!openFiles[i].used)
			return;
		  		//fprintf(stderr, "File not open to be closed!\n");

		// only delete the file from the list of open files if you are the last one using it
		openFiles[i].refCount--;
		if ( (openFiles[i].refCount) == 0 ) {
			openFiles[i].used = 0;
			openFiles[i].name = NULL;
			openFiles[i].openFile = NULL;
			currentThread->openFilesMap->Clear(i);
		}
	}

}

// ----------------------------------------------------------------------
//
// Called as result of the exit syscall. Returns the exit status that 
// is passed in. Kills the thread that called exit. 
//
// ----------------------------------------------------------------------
void exit() {
  // get exitstatus 
  int exitStatus = machine->ReadRegister(4);
  // if non-zero, needs to be positive
  if (exitStatus < 0) {
    DEBUG('a', "Exit values need to be non-negative\n");
    exitStatus = 1;
  }
  DEBUG('s', "Exiting with status %d\n", exitStatus);
  // set status in process to done

  // attempt to close all open files
  if (currentThread->openFilesMap != NULL) {
    for( int i =  2; i < NumOpenFiles ; i++) {
      if (currentThread->openFilesMap->Test(i)){
	exitCloseFile(i);
      }
    }	
  }
  delete currentThread->openFilesMap;
  // get mutex on processinfo, because we don't want child
  // changing the status on the parent.
  procLock->Acquire();
  ProcessInfo * child;

  // for all children, if they are done running, delete their
  // processinfo
  while ((child = currentThread->procInfo->GetChild()) != NULL ) {
    if (child->GetStatus() == DONE) {
      delete child;
    }
    // if not done, make sure the child knows to 
    // take care of itself later
    else
      child->setStatus(ZOMBIE);
  }
  
  // like here!
  if (currentThread->procInfo->GetStatus() == ZOMBIE) {
    delete currentThread->procInfo;
  }

  // When a child is done, it wakes a parent if there is
  // one waiting to join.
  else {
    currentThread->procInfo->setStatus(DONE);
    
    currentThread->procInfo->setExitStatus(exitStatus);
    currentThread->procInfo->WakeParent();
  }
  
  // release the mutex on the processinfo
  procLock->Release();

  // remove address space of any page in memory
  for (int i = 0; i < NumPhysPages; i++) {
    physPageDesc[i].pageLock->Acquire();
    if (physPageDesc[i].space == currentThread->space) {

      physPageDesc[i].space = NULL;
      physPageDesc[i].valid = false;
      physPageDesc[i].dirty = false;

    }
    physPageDesc[i].pageLock->Release();
  }
  
  // toss the addrspace so we have memory to run more programs.
  delete currentThread->space;
  currentThread->space = NULL;
  currentThread->Finish();
  
}

/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */

//----------------------------------------------------------------------
// The function that handles the system call Join()
//----------------------------------------------------------------------

void joinProcess() {
  // get the pid of the process to be joined
  SpaceId joinId = machine->ReadRegister(4);
  
  // actual joining done in processinfo, where the parent waits 
  // on a condition variable
  // don't need mutex here because each function in processinfo
  // has a lock.
  int exitStatus = currentThread->procInfo->ProcessJoin(joinId);

  machine->WriteRegister(2, exitStatus);
}

//----------------------------------------------------------------------
// checkPoint
//     creates a checkpoint of the program.
//----------------------------------------------------------------------

void checkPoint() {

  char * filename = new(std::nothrow) char[128];
  whence = machine->ReadRegister(4);

  // get char * address from information at argv, then argv+1 etc.
  AddrSpace *space = currentThread->space;
  int physAddr;

  // get filename from physical memory
  for ( int i = 0 ; i < 127 ; i++) {
    LockPage(whence + i);
    
    if  ( !space->memManager->Translate(whence + i, &physAddr, 1, false, space)) {
      machine->WriteRegister(2,-1);
      ReleasePage(whence + i);
      return;
    }
    ReleasePage(whence + i);
    if ((filename[i] = machine->mainMemory[physAddr]) == '\0') break;
  }
  filename[127] = '\0';
  
  // if we are unable to create this new file, checkpoint should return -1
  if ( ! fileSystem->Create(filename, 0) ) {// second arg not needed, dynamic file size
    machine->WriteRegister(2, -1);
    return;
  }

  OpenFile *cp = fileSystem->Open(filename);

  // or if we are unable to open this new file
  if (cp == NULL) {
    machine->WriteRegister(2, -1);
    return;

  }

  //get the magic checkpoint number and all the registers
  // and write these to the checkpoint file
  int cpNum = CPNUMBER;
  cp->Write((char *) &cpNum, 4);
  for (int i = 0; i < NumTotalRegs; i++) {
    regs[i] = machine->ReadRegister(i);
    cp->Write((char *) &regs[i], sizeof(int));
  }

  char buffer[128];
  int numPages = space->numPages;

  // for each virtual page in the address space
  for (int i = 0; i < numPages; i++) {
    // if the page is not on the disk we should load it to the disk in order to
    // write it to a file
    int physicalPage = space->pageTable[i].physicalPage;
    if (space->pageTable[i].diskPage == -1) space->LoadPageToDisk(i);

    //if the page is dirty, write it back to disk
    else if (physicalPage != -1) {
      WriteDirtyPage(physicalPage);
    }
    
    disk->ReadSector(space->pageTable[i].diskPage, buffer);
    cp->Write( buffer, 128);
  }
  
  // return 0 if the checkpoint is done properly
  machine->WriteRegister(2, 0);
}

#endif

//----------------------------------------------------------------------
// HandleTLBFault
//      Called on TLB fault. Note that this is not necessarily a page
//      fault. Referenced page may be in memory.
//
//      If free slot in TLB, fill in translation info for page referenced.
//
//      Otherwise, select TLB slot at random and overwrite with translation
//      info for page referenced.
//
//----------------------------------------------------------------------
#ifdef CHANGED
void
HandleTLBFault(int vaddr)
{
  int vpn = vaddr / PageSize;
  int victim = Random() % TLBSize;
  int i;

  stats->numTLBFaults++;
  // need to make sure we are referencing within our address space
  // Otherwise, kill the process
  if (vaddr < 0 || vaddr > currentThread->space->MaxVirtualAddress) {
    // here we kill the process
    // Return exit code of 1
    // indicating abnormal exit
    machine->WriteRegister(4, 1);
    exit();
  }

  LoadPageToMemory(vpn, currentThread->space);

  // First, see if free TLB slot
  for (i=0; i<TLBSize; i++)
    if (machine->tlb[i].valid == false) {
      victim = i;
      break;
    }
	
  // Otherwise clobber random slot in TLB

  // need to get correct physical page
  PageInfo * table = currentThread->space->getPageTable();
  // the ith entry is always the ith virtual page

  machine->tlb[victim].virtualPage = vpn;
  machine->tlb[victim].physicalPage = table[vpn].physicalPage;
  machine->tlb[victim].valid = true;
  machine->tlb[victim].dirty = false;
  machine->tlb[victim].use = false;
  machine->tlb[victim].readOnly = false;
}

#endif
#endif

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch (which) {
      case SyscallException:
	switch (type) {
	  case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
            interrupt->Halt();

#ifdef CHANGED

	  case SC_Create:
	    {
	      createNewFile();
	      incrementPC();
	      break;
	    }			
	  case SC_Open:
	    {	
	      if (openFiles == NULL) {
		for (int j = 2 ; j < 10 ; j++) { 
		  //openFiles[j].openFile = new(std::nothrow) OpenFile(j);
		  openFiles[j].used = 0;
		}
	      }
	      openFile();
	      incrementPC();
	      break;
	    }		
	  case SC_Write:
	    {
	      writeFile();
	      incrementPC();
	      break;
	    }
	  case SC_Read:
	    {
	      readFile();
	      incrementPC();
	      break;
	    }
	  case SC_Close:
	    {
	      closeFile();
	      incrementPC();
	      break;
	    }

	  case SC_Exit:
	    exit();
	    break;

	  case SC_Exec:
	    execFile();
	    incrementPC();
	    break;

	  case SC_Join:
	    joinProcess();
	    incrementPC();
	    break;

	    //exception system call
	  case SC_CheckPoint:
	    checkPoint();
	    incrementPC();
	    break;
	    

	    /*	  case SC_Fork:

	    forkProgram();
	    incrementPC();
	    break;

	  case SC_Yield:

	    incrementPC();
	    yieldProgram();
	    break;
	    */
#endif
      default:
	    printf("Undefined SYSCALL %d\n", type);
	    ASSERT(false);
	}
#ifdef USE_TLB
      case PageFaultException:
	HandleTLBFault(machine->ReadRegister(BadVAddrReg));
	break;
#endif
      default: ;
    }
}
