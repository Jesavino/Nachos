#ifdef CHANGED

#include "elevatorTest.h"
#include "system.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
//
// Here beings the implementation of the elevator test. The method used an
// ElevatorManager to manage the movement of the elevator. The elevator
// itself is a thread in elevatorThread(). In its implementation, the 
// elevator goes to sleep until someone calls it. It will serve until no more 
// calls are made. The main work is done in the Travel() method, which is
// detailed in the ElevatorManager class. 
// 
// The number of people and number of floors are both passed in as arguments
// to the program. The randomization of both the starting floors and finish 
// floors is done using a seed we chose to be 10. 
// 
// -----------------------------------------------------------------------------

// Initialize globals
ElevatorManager *manager;
char personName[12];
int floorCount = numberOfFloors;

// These two are passed into the Manager in order to allow mutex and waits in
// the threads utilizing the manager. 
Condition *elevator;
Lock * mutex;

// -----------------------------------------------------------------------------
//
// The person thread is called with a simple ID to identify different people. 
// The start and finish floors are implemented randomly. There is a simple check 
// to see if the floors are the same, and this problem is fixed by incrementing
// the finish floor. 
// The person "states" their intent to travel via a call to ArrivingGoingFromTo. 
// The documentation for this function is in ElevatorManager. 
// Upon finishing, each person makes sure the elevator wakes up. If it is already
// awake, this is a no-op, otherwise, the elevator checks for people waiting, and 
// eventually returns to sleep. 
//
// -----------------------------------------------------------------------------
void personThread(int id) {
	// initialize all variables and perform random floor assignments
	int curFloor;
	int start = curFloor = rand() % floorCount;
	int finish = rand() % floorCount;
	if (start == finish) 
		finish = (finish + 1 ) % floorCount;

	printf("I am person %d and I am calling from floor %d going to %d\n", id , curFloor , finish );
	manager->ArrivingGoingFromTo(start , finish , id );
	elevator->Signal(mutex);
}

// -------------------------------------------------------------------------------
// 
// The elevator thread first checks if people are waiting. If no people are, it
// will just go to sleep. Each person "wakes" the elevator in their call to 
// ArrivingGoingFromTo. Otherwise, the elevator begins Travel(). More documentation
// for Travel() is in ElevatorManager, but in short, the elevator will only return 
// back to this thread once all Travel has been executed. 
//
//---------------------------------------------------------------------------------
void elevatorThread(int id) {
	while(1) {
		mutex->Acquire();
		if (!manager->checkIfPeopleWaiting() ) {
			printf("Elevator Sleeping on Mutex\n");
			elevator->Wait(mutex);
		}
		printf("Elevator working on requests!\n");
		mutex->Release();
		manager->Travel();
	}
}
// ---------------------------------------------------------------------------------
// 
// Basic constructor for the test. Locks, Conditions, and the Manager are all 
// created here. srand() sets the random seed for random number generation. This
// number can be changed arbitrarily. 
//
// The number of people and floors of the elevator are both created here. 
//
// TEST FOR CORRECTNESS:
// In order to prove correctness of this test, one should verify the output by 
// observing that the output is correct. For example, Each person will state when 
// they call the elevator, and when they get off the elevator. Using this information,
// it is easy to see that (with knowledge that the elevator starts on floor 0), each
// person is served in the correct order. All upsweeps first, and then downsweeps.
// 
// ----------------------------------------------------------------------------------

void elevatorTest( int numPeople )  {

	printf("-------------------------------------------------------------\n");
	printf("-\n- Thus begins the solution to the elevator problem\n");
	printf("-\n-------------------------------------------------------------\n");

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
