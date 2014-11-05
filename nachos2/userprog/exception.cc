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
//----------------------------------------------------------------------
// Space for Global Data as needed
//
//----------------------------------------------------------------------

static const int numOpenFiles = 20;

int pid = 1;
int argc;
char *args[10];
AddrSpace *argSpace; // new space for args
struct openFileDesc {
	OpenFile *openFile = NULL;
	int used;
	int refCount;
	char * name;
}openFiles[numOpenFiles];

char * stringarg;
int whence;
SynchConsole *synchConsole;
Lock * procLock = new(std::nothrow) Lock("global process lock");

// Increments the program counters
void incrementPC() {

	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, pc);
	pc += 4;
	machine->WriteRegister(NextPCReg, pc);
}
// create a new file
void createNewFile() {
	int physAddr;
	DEBUG('a', "Creating a new file.\n");
	stringarg = new (std::nothrow) char[128];
	whence = machine->ReadRegister(4);
	AddrSpace *space = currentThread->space;
	fprintf(stderr, "File name begins at address %d in user VAS\n" , whence);
	/*  Old working version w/ 1to1 mem 
	for (int i = 0 ; i < 127 ; i++) 
		if ((stringarg[i] = machine->mainMemory[whence++]) == '\0') break;
	*/
	for ( int i = 0 ; i < 127 ; i++) {
		if( ! space->memManager->Translate(whence, &physAddr, 1, false, space) ) {
			fprintf(stderr, "Bad Translation\n");
			break;
		}
		if ((stringarg[i] = machine->mainMemory[physAddr]) == '\0') break;		 
		whence++;
	}
	// */
	stringarg[127] = '\0';

	fprintf(stderr, "File creation attempt on filename %s\n" , stringarg);
	if ( ! fileSystem->Create(stringarg, 0) ) // second arg not needed, dynamic file size
		fprintf(stderr, "File Creation Failed. Either the file exists or there are memory problems\n");

	//fprintf(stderr, "File Creation Successful. Returning\n");
}
// open a file
void openFile() {
	int physAddr;
	AddrSpace *space = currentThread->space;
	DEBUG('a', "Opening file.\n");
	stringarg = new(std::nothrow) char[128];
	whence = machine->ReadRegister(4);

	//fprintf(stderr, "File name begins at address %d in user VAS\n" , whence);
	/*
	for (int i = 0 ; i < 127 ; i++)
		if ((stringarg[i] = machine->mainMemory[whence++]) == '\0') break;
	stringarg[127] = '\0';
	*/
	for ( int i = 0 ; i < 127 ; i++ ) {
		if ( !space->memManager->Translate(whence, &physAddr, 1, false, space) ) {
			fprintf(stderr, "Bad Translation\n");
			break;
		}
		if ((stringarg[i] = machine->mainMemory[physAddr]) == '\0') break;
		whence++;
	}
	stringarg[127] = '\0';

	//fprintf(stderr, "Attempting to open filename %s\n", stringarg);
	OpenFile *file = fileSystem->Open(stringarg);
	if (file == NULL)
		fprintf(stderr, "Error during file opening\n");
	//else 
	//	fprintf(stderr, "File Opened Successfully. Returning\n");

	// We need to place the file in a Kernel accessable space?
	// so we find the space to put the file. 
	int fileId = -1;
	for ( int i = 2 ; i < numOpenFiles ; i++) {
		if (! openFiles[i].used ) {
			openFiles[i].used = 1;
			openFiles[i].refCount++;
			openFiles[i].name = stringarg;
			openFiles[i].openFile = file;
			fileId = i;
			break;
		}
	}
	if (fileId == -1)
		fprintf(stderr, "Too many files open\n");
	else
		machine->WriteRegister(2, fileId);

}
// write to the specified file
void writeFile() {
	int physAddr;
	AddrSpace *space = currentThread->space;
	DEBUG('a', "Writing to a file\n");
	
	int size = machine->ReadRegister(5);
	whence = machine->ReadRegister(4);

	stringarg = new(std::nothrow) char[size + 1];
	//for (int i = 0 ; i < size ; i++)
	//	if ((stringarg[i] = machine->mainMemory[whence++]) == '\0') break;
	//fprintf(stderr, "Attempting to write string %s to file\n" , stringarg);
 	
	for (int i = 0 ; i < size ; i++) {
		if ( ! space->memManager->Translate(whence + i, &physAddr, 1, false, space)) {
			fprintf(stderr, "Error in Write Translation\n");
			break;
		}
		if ((stringarg[i] = machine->mainMemory[physAddr]) == '\0') break;
	}

	stringarg[size] = '\0';
	int file = machine->ReadRegister(6);

	// Here we will add writing to the console specifically
	// 	if (console == NULL) console = new(std::nothrow)
	if ( file == ConsoleOutput ) {
		if (synchConsole == NULL) synchConsole = new(std::nothrow) SynchConsole(NULL, NULL);
		synchConsole->WriteLine(stringarg);
		
	}
	else if ( file == ConsoleInput )
		fprintf(stderr, "Cannot Write to StdInput\n");
	else {
		if( !openFiles[file].used )
			fprintf(stderr, "Requested file has not been opened!\n");
		OpenFile *fileToWrite = openFiles[file].openFile;
		int numWrite = fileToWrite->Write( stringarg, size);
		machine->WriteRegister(2, numWrite);

	}	
}
// read from the specified file
void readFile() {
	DEBUG('a' , "Reading from File\n");
	AddrSpace* space = currentThread->space;
	int file = machine->ReadRegister(6);
	if (file < 0 || file > 9) {
		fprintf(stderr, "Invalid file read attempt\n");
	}
	int numBytes = machine->ReadRegister(5);
	// in this case, whence is mainMemory address of buffer to read into
	whence =  machine->ReadRegister(4);	

	if (file == ConsoleInput) {
		char * readChar = new(std::nothrow) char[128];
		if (synchConsole == NULL) synchConsole = new(std::nothrow) SynchConsole(NULL, NULL);
		for( int i = 0 ; i < numBytes ; i++) {
			readChar[i] = synchConsole->SynchGetChar();
			if ( ! space->memManager->WriteMem(whence + i , 1, (int) readChar[i], space)) {
				fprintf(stderr, "Error writing to StdInput\n");
				break;
			}
		}
		//readChar[127] = '\0';
		 
		
		//strcpy(&machine->mainMemory[whence] , readChar);
		machine->WriteRegister(2, numBytes);
	}
	else if(!openFiles[file].used){
		// error reading from closed or non-existing file
		machine->WriteRegister(2, -1);
	}
	else {
	
		char * buffer = (char *) malloc( sizeof(char*) * numBytes );
		OpenFile *fileToRead = openFiles[file].openFile;

		int numRead = fileToRead->Read( buffer , numBytes );
		for ( int i = 0 ; i < numBytes ; i++) {
			if ( !space->memManager->WriteMem(whence + i , 1, (int) buffer[i],space)) {
				fprintf(stderr, "Error reading from file in memory write\n");
				break;
			}
		}
		//int len = strlen(buffer);
		//strcpy(&machine->mainMemory[whence] , buffer);
		machine->WriteRegister(2 , numRead);
	}
}
// close the file
void closeFile() {
	DEBUG('a', "Closing the file\n");

	int file = machine->ReadRegister(4);

	if (file == ConsoleInput)
		fprintf(stderr, "Cannot close Console Input\n");
	else if (file == ConsoleOutput)
		fprintf(stderr, "Cannot close Console Output\n");
	else {
		if (!openFiles[file].used)
			fprintf(stderr, "File not open to be closed!\n");
		
		// only delete the file from the list of open files if you are the last one using it
		openFiles[file].refCount--;
		if ( (openFiles[file].refCount) == 0 ) {
			openFiles[file].used = 0;
			openFiles[file].name = NULL;
			openFiles[file].openFile = NULL;
		}
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

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void prepStack(int argcount, char **argv, AddrSpace *space) {
	int sp;
	int len;
	int argvAddr[argcount];
	int physAddr;
	char * tmp;
	for(int i = 0 ; i < argcount ; i++)
		fprintf(stderr, "Argv[%d] = %s\n", i, argv[i]);
	sp = machine->ReadRegister(StackReg);
	fprintf(stderr, "StackPointer = %d\n", sp);
	for ( int i = 0 ; i < argcount ; i++) {
		len = strlen(argv[i]) + 1;
		sp -= len;
		tmp = argv[i];
		fprintf(stderr, "tmp is %s\n", tmp);
		for ( int j = 0; j < len ; j++) {
			space->memManager->Translate(sp + j, &physAddr, 1, false, space);
			machine->mainMemory[physAddr] = tmp[j];
			if(j == 0) argvAddr[i] = physAddr;
		}
		

	}
	// align SP
	sp = sp & ~3;

	// Allocate and fill the argv array
	sp -= sizeof(int) * argcount;

	for ( int i = 0 ; i < argcount; i++ ) {
		space->memManager->Translate(sp + i*4, &physAddr, 4, false, space);
		*(unsigned int *) &machine->mainMemory[physAddr] = WordToMachine((unsigned int) argvAddr[i]);
		/*
		*(unsigned int *) &machine->mainMemory[sp + i*4]
			= WordToMachine((unsigned int) argvAddr[i]);*/
	}
	fprintf(stderr, "Argc is %d SP is %d\n", argcount, sp);
	machine->WriteRegister(4, argcount);
	machine->WriteRegister(5, sp);

	machine->WriteRegister(StackReg, sp - 8);

}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void execThread(int arg) {
  
  currentThread->space->InitRegisters();		// set the initial register values
  currentThread->space->RestoreState();		// load page table register
  if(argc) 
		prepStack(argc, args, argSpace);

  machine->Run();			// jump to the user progam
  ASSERT(false);			// machine->Run never returns;
                                        // the address space exits
                                        // by doing the syscall "exit"

}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void execFile() {
	argc = 0; // set number of args to 0 until we check
  char * filename = new(std::nothrow) char[128];
  whence = machine->ReadRegister(4);
  // get char * address from information at argv, then argv+1 etc.
  AddrSpace *newSpace;
  AddrSpace *space = currentThread->space;
 
  int physAddr;
  fprintf(stderr, "File name begins at address %d in user VAS\n" , whence);

  
	// Address translation

  /* Removing one to one mapping
     for (int i = 0 ; i < 127 ; i++)
     if ((filename[i] = machine->mainMemory[whence++]) == '\0') break;
  */

  for ( int i = 0 ; i < 127 ; i++) {
    if  ( !space->memManager->Translate(whence + i, &physAddr, 1, false, space)) {
      fprintf(stderr, "Invalid translate to exec'd file addr\n");
      break;
    }
    if ((filename[i] = machine->mainMemory[physAddr]) == '\0') break;
  }
  filename[127] = '\0';

	// Get arguments from the kernel
	char * argv[10] = {NULL};
	char * tmp = new(std::nothrow) char[128];
	int arg;
	whence = machine->ReadRegister(5);
	space->memManager->ReadMem(whence, 4, &arg, space);
	int j = 0;
	int size;
	while(arg != 0) {
		for ( int i = 0 ; i < 127 ; i++) {
			if((tmp[i] = machine->mainMemory[arg+i]) == '\0') break;
		}
		tmp[127] = '\0';
		size = sizeof(char) * (strlen(tmp)+1); // null terminator
		argv[j] = new(std::nothrow) char[size];
		strcpy(*(argv + j) , tmp);		
		j++;		
		whence += 4;
		space->memManager->ReadMem(whence, 4, &arg, space);
	}
	for (int i  = 0 ; i < 10 ; i++) {
		if( argv[i] == NULL) break;
		args[i] = argv[i];
		fprintf(stderr, "Argv[%d] is %s\n", i, argv[i]);
	}
	// set global data for prep
	argc = j;

	fprintf(stderr, "Attempting to open filename %s\n", filename);
  OpenFile *executable = fileSystem->Open(filename);
  
  if (executable == NULL) {
    printf("Unable to open file %s\n", filename);
    machine->WriteRegister(2, -1);
    return;
  }

  newSpace = new(std::nothrow) AddrSpace(executable);    
  Thread * thread = new(std::nothrow) Thread("execed thread");
  thread->space = newSpace;
  procLock->Acquire();
  thread->pid = pid++;
  thread->procInfo = new(std::nothrow) ProcessInfo(thread->pid, currentThread->pid);
  currentThread->procInfo->AddChild(thread->procInfo);
  procLock->Release();
  // calling thread given this threads pid.
  // put it in thread?
  //printf("%d\n", thread->pid);
  machine->WriteRegister(2, thread->pid);
  
	// If there were args, prep the stack here
	argSpace = newSpace;
  thread->Fork(execThread, 0);

	
  delete executable;			// close file


}  

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void exit() {
  int exitStatus = machine->ReadRegister(4);
  if (exitStatus < 0) {
    DEBUG('a', "Exit values need to be non-negative\n");
    exitStatus = 1;
  }
  DEBUG('s', "Exiting with status %d\n", exitStatus);
  // set status in process to done
  procLock->Acquire();
  ProcessInfo * child;
  while ((child = currentThread->procInfo->GetChild()) != NULL ) {
    if (child->GetStatus() == DONE) {
      delete child;
    }
    else
      child->setStatus(ZOMBIE);
  }
  if (currentThread->procInfo->GetStatus() == ZOMBIE) {
    delete currentThread->procInfo;
  }
  else {
    currentThread->procInfo->setStatus(DONE);

    currentThread->procInfo->setExitStatus(exitStatus);
    currentThread->procInfo->WakeParent();
  }
  procLock->Release();
  delete currentThread->space;
  currentThread->Finish();

}

/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */

//----------------------------------------------------------------------
//----------------------------------------------------------------------

void joinProcess() {
  SpaceId joinId = machine->ReadRegister(4);

  // delete processinfo for each child that is joined.
  // because once they are joined, we do not need them anymore.
  //procLock->Acquire();
  int exitStatus = currentThread->procInfo->ProcessJoin(joinId);
  //procLock->Release();
  //dummy return for now
  machine->WriteRegister(2, exitStatus);
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

  // First, see if free TLB slot
  for (i=0; i<TLBSize; i++)
    if (machine->tlb[i].valid == false) {
      victim = i;
      break;
    }
	
  // Otherwise clobber random slot in TLB

	// need to get correct physical page
	TranslationEntry * table = currentThread->space->getPageTable();
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
