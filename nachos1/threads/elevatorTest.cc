#ifdef CHANGED

#include "elevator_manager.h"
#include "system.h"

int numPeople = 3;

ElevatorManager *manager;
char personName[12];

void personThread(int id) {
	int i;
	int curFloor = id; // each person starts on a different floor
	int count = 0;
	for( i = curFloor ; count < 3; i = (i + 1)%3) {
		printf("I am person %d and I am calling from floor %d\n", id , curFloor);
		manager->ArrivingGoingFromTo(i , curFloor = (i+1)%3);
		printf("I am person %d and I got off on floor %d\n", id , curFloor);
		manager->DepartingAt(curFloor);
		count++;
		currentThread->Yield();

	}


}

void elevatorThread(int id) {
	// in this implementation the elevator is always running
	

}

void elevatorTest() {
	manager = new ElevatorManager();
	int i;
	Thread *people[numPeople];

	for( i = 0; i < numPeople; i++) {
		
		sprintf(personName , "%s%d", "person" , i);
		people[i] = new(std::nothrow) Thread(personName);
		
		people[i]->Fork(personThread , i);
	}
	
	elevatorThread(1);
	
}
#endif
