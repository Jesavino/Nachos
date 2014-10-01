#ifdef CHANGED

#include "elevatorTest.h"
#include "system.h"
#include <stdlib.h>

int numPeople = 3;
Condition * elevator;
ElevatorManager *manager;
char personName[12];
Lock * mutex;

void personThread(int id) {
	int i;
	int curFloor = id; // each person starts on a different floor
	//int count = 0;
	int start = curFloor = rand() % 5;
	int finish = rand() % 5;
	if (start == finish) 
		finish = (finish+1)%5;
		printf("I am person %d and I am calling from floor %d going to %d\n", id , curFloor , finish );
		manager->ArrivingGoingFromTo(start , finish );
		curFloor = finish;
		printf("I am person %d and I got off on floor %d\n", id , curFloor);
		//count++;
		//currentThread->Yield();

}

void elevatorThread(int id) {
	
	while(1) {
		mutex->Acquire();
		printf("Elevator Sleeping on Mutex\n");
		elevator->Wait(mutex);
		printf("Elevator Has woken up!\n");
		mutex->Release();
		manager->Travel();



	}
	

}

void elevatorTest() {
	mutex = new(std::nothrow) Lock("Elevator Lock");
	elevator = new(std::nothrow) Condition("Elevator Condition");
	manager = new ElevatorManager( mutex , elevator );
	int i;
	Thread *people[numPeople];
	srand(10); 


	for( i = 0; i < numPeople; i++) {
		
		sprintf(personName , "%s%d", "person" , i);
		people[i] = new(std::nothrow) Thread(personName);
		
		people[i]->Fork(personThread , i);
	}
	
	elevatorThread(1);
	
}
#endif
