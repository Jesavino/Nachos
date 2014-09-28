#ifdef CHANGED
#include "prioritytest.h"
#include "copyright.h"
#include "system.h"

//---------------------------------------------------------
//
// SimpleThread to be created upwards of 20 times to 
// test the priority scheduling
//
//---------------------------------------------------------

void
PriorityThread(int processName) {
	int num;

	for ( num = 0 ; num < 5 ; num++ ) {
		printf("*** Thread %d looped %d times!\n", processName , num);
		currentThread->Yield();
	}
}

void
PriorityTest() {

	DEBUG('t', "Entering PriorityTest");
	List *toGo = new List;
	int num;
	
	// create 5 processess with individual priorities
	for ( num = 0 ; num < 5 ; num++ ) {
		Thread *t = new(std::nothrow) Thread((char *) num , num);
		toGo->Append((void *)t);
	}
	num = 0;
	Thread *t;
	while (	(t = (Thread *)toGo->Remove()) != NULL ){
		t->Fork(PriorityThread , num);
		num++;
	}
	PriorityThread(5);
}
#endif
