#ifdef CHANGED
#include "elevator_manager.h"

ElevatorManager::ElevatorManager(Lock * eMutex, Condition * eCond ) {
	pos = 0;
	directionUp = 1; //initialized to 1st floor going up
	upcount = downcount = 0;
	int i;
	elevator = eCond;

	// initialize all the conditions and ints
	for (i = 0 ; i < numberOfFloors ; i++) {
		upsweep[i] = new(std::nothrow) Condition("UP CONDITION");
		downsweep[i] = new(std::nothrow) Condition("DOWN CONDITION");
		upwait[i] = downwait[i] = dest[i] = 0;
	}
	mutex = eMutex;

}
// --------------------------------------------------------------------------
//
// ARGS:
// 		- aFloor is the floor the person calling the elevator is calling from
//		- bFloor is the floor the person would like to go to
//		- id is the id of the person calling the elevator
//
// This function works in two parts. The first part determines if the person
// calling the function would like to go up or down, based on calling floor
// and finish floor. Using this information, the manager increments counts 
// and puts the person to sleep on the correct floor
//
// Once woken, the manager gets the destination info from the person, 
// and puts the person to sleep while they wait for the elevator to get there. 
// Once the person  has arrived at their destination, they are awoken, they
// tell the elevator they are off, and they leave the elevator. 
//
// ------------------------------------------------------------------------------
void
ElevatorManager::ArrivingGoingFromTo(int aFloor , int bFloor, int id) {

	// Person is at the elevator, and elevator is proccessing their request
	DEBUG('t' , "Arriving in Elevator Manager");
	elevator->Signal(mutex);
	mutex->Acquire();
	if ( (pos <= aFloor) && (aFloor < bFloor) ) {
		upcount++;
		upwait[aFloor]++;
		upsweep[aFloor]->Wait(mutex);
	} else {
		downcount++;
		downwait[aFloor]++;
		downsweep[aFloor]->Wait(mutex);
	}

	printf("I am person %d getting on the elevator at floor %d\n" , id , aFloor);
	// Person has woken up and is on the elevator
	DEBUG('t' , "Woke Up Person From Mutex");
	dest[bFloor]++;
	elevator->Signal(mutex);
	if(directionUp)
		upsweep[bFloor]->Wait(mutex);
	else
		downsweep[bFloor]->Wait(mutex);

	// Person is at destination. Say so, and tell the elevator to move on
	elevator->Signal(mutex);
	printf("I am person %d getting off at floor %d\n", id, bFloor );
	mutex->Release();
}

// ---------------------------------------------------------------------------------
//
// Travel() is responsible for move the elevator from floor to floor. The algorithm
// first checks to see if there are people in the direction it is going. If there 
// are, the elevator serves them first. Otherwise, the elevator changes direction. 
// If there are no people in either direction, the elevator goes to sleep, to be 
// awoken by the next person who needs the elevator. 
//
// Travel() is called again at the end of every movement to check if there are any 
// people waiting. 
//
// ----------------------------------------------------------------------------------
void 
ElevatorManager::Travel() {
	mutex->Acquire();

	// Case where elevator is moving up
	if (directionUp) {
		
		// if there are people going / waiting above us
		if (upcount) {
			next = find_nextUp(pos);		
			printf("Elevator moving from floor %d to floor %d\n" , pos , next);
			pos = next;
			upcount -= dest[pos];
			upwait[pos] = 0;
			dest[pos] = 0;
			upsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			mutex->Release();
			Travel();

		// otherwise, change direction
		} else if (downcount) {
			directionUp = 0;
			next = find_nextDown(pos);
			printf("Elevator moving from floor %d to floor %d\n" , pos, next);
			pos = next;
			downcount -= dest[pos];
			downwait[pos] = 0;
			dest[pos] = 0;
			downsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			mutex->Release();
			Travel();
		}
	}
	
	// Case where the elevator is going down
	else {
		// if there are people above us but nobody below
		if (!downcount && upcount) {
			directionUp = 0;
			next = find_nextUp(pos);
			printf("Elevator moving from floor %d to floor %d\n" , pos, next);
			pos = next;
			upcount -= dest[pos];
			upwait[pos] = 0;
			dest[pos] = 0;
			upsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			mutex->Release();
			Travel();

		// otherwise, continue going down
		} else if (downcount) { 
			next = find_nextDown(pos);
			printf("Elevator moving from floor %d to floor %d\n", pos, next);
			pos = next; 
			downcount -= dest[pos];
			downwait[pos] = 0;
			dest[pos] = 0;
			downsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			mutex->Release();
			Travel();
		}
	}
	// if nobody is waiting, go to sleep
	if ( !checkIfPeopleWaiting() ) 
		elevator->Wait(mutex);		
}
// --------------------------------------------------------------
//
// find_nextUp uses the current position of the elevator to 
// find the next person waiting or destination above the 
// elevators current position. When this function is called, 
// it is guaranteed that there will be either a person calling or 
// person looking to get off above. 
//
// ----------------------------------------------------------------
int
ElevatorManager::find_nextUp(int position) {
	int i;
	int min = INT_MAX;

	for(i = position ; i < numberOfFloors ; i++) {
		if(upwait[i]) {
			 min = i;
			break;
		}
	}
	
	for (i = position  ; i < numberOfFloors ; i++) {
		if( dest[i] && i < min) return i; 
	}
	return min;
}
// ----------------------------------------------------------------------
//
// Similar to find_nextUp(), this function finds the next person waiting 
// either to be picked up or to be dropped off below the current 
// elevators position. As before, there is guaranteed to be someone below
// the current position. 
//
// ------------------------------------------------------------------------
int 
ElevatorManager::find_nextDown(int position) {
	int i;
	int max = INT_MIN;

	for (i = position  ; i >= 0; i--) {
		if(downwait[i]) {
			max = i;
			break;
		}
	}
	for (i = position ; i >= 0; i--) {
		if(dest[i] && i > max) return i;
	}
	return max;
}
// --------------------------------------------------------------------------
//
// A simple method to debug counters and make sure they were being updated
// appropriately. Useful for seeing where the program halted at times
//
// ---------------------------------------------------------------------------
void
ElevatorManager::debugPrint() {
	int i;
	for(i = 0 ; i < numberOfFloors ; i++ )
		printf("upwait[%d] = %d downwait[%d] = %d dest[%d] = %d\n" ,
			i, upwait[i] ,i , downwait[i] ,i, dest[i]);
}
// -----------------------------------------------------------------------------
//
// A check to see if anyone is waiting. Used to determine if the elevator should
// sleep or serve to start and throughout the execution
// 
// -------------------------------------------------------------------------------
int 
ElevatorManager::checkIfPeopleWaiting() {
	return upcount + downcount;
}
#endif
