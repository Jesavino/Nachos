#include "elevator_manager.h"

ElevatorManager::ElevatorManager(Lock * eMutex, Condition * eCond  ) {
	pos = 0;
	directionUp = 1; //initialized to 1st floor going up
	busy = 0; // not busy
	upcount = downcount = 0;
	numFloors = 5;
	int i;
	elevator = eCond;
	for (i = 0 ; i < numFloors ; i++) {
		upsweep[i] = new(std::nothrow) Condition("UP CONDITION");
		downsweep[i] = new(std::nothrow) Condition("DOWN CONDITION");
		upwait[i] = downwait[i] = dest[i] = 0;
	}
	mutex = eMutex;

}
void
ElevatorManager::ArrivingGoingFromTo(int aFloor , int bFloor, int id) {
	DEBUG('t' , "Arriving in Elevator Manager");
	elevator->Signal(mutex);
	mutex->Acquire();
	if ( (pos <= aFloor) && (aFloor < bFloor) ) {
		upcount++;
		upwait[aFloor]++;
		//printf("About to sleep on upsweep[%d]\n" , aFloor);
		//debugPrint();
		upsweep[aFloor]->Wait(mutex);
	} else {
		downcount++;
		downwait[aFloor]++;
		//debugPrint();
		downsweep[aFloor]->Wait(mutex);
	}
	DEBUG('t' , "Woke Up Person From Mutex");
	printf("Before mutex in elevator\n");
	dest[bFloor]++;
	//debugPrint();
	elevator->Signal(mutex);
	if(directionUp)
		upsweep[bFloor]->Wait(mutex);
	else
		downsweep[bFloor]->Wait(mutex);
	//printf("Signaling Elevator\n");
	elevator->Signal(mutex);
	printf("I am person %d getting off at floor %d\n", id, bFloor );
	mutex->Release();

}
void 
ElevatorManager::Travel() {
	mutex->Acquire();
	//printf("Got Mutex in Travel()\n");
	//debugPrint();
	if (directionUp) {
		//printf("We see that Up: %d , Down: %d\n" , upcount , downcount);
		if (upcount) {
			//printf("Going Up!\n");
			next = find_nextUp(pos);		
			pos = next;
			//printf("Moving to %d\n" , pos);
			upcount -= dest[pos];
			upwait[pos] = 0;
			dest[pos] = 0;
			//printf("Waking up Upsweep[%d]\n", pos);
			upsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			//printf("Elevator moving again\n");
			mutex->Release();
			Travel();
		} else {
			//printf("Changing Dir, going down!\n");
			directionUp = 0;
			next = find_nextDown(pos);
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
	else if (downcount) {
		if (upcount) {
			//printf("Changing Direction, going up!\n");
			directionUp = 0;
			next = find_nextUp(pos);
			pos = next;
			upcount -= dest[pos];
			upwait[pos] = 0;
			dest[pos] = 0;
			upsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			mutex->Release();
			Travel();
		} else {
			//printf("Going down!\n");
			next = find_nextDown(pos);
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
	if (upcount + downcount == 0) 
		elevator->Wait(mutex);		
	
}
int
ElevatorManager::find_nextUp(int position) {
	int i;
	int min = INT_MAX;

	for(i = position ; i < numFloors ; i++) {
		if(upwait[i]) {
			 min = i;
			//printf("Min is: %d\n" , min);
			break;
		}
	}
	
	for (i = position  ; i < numFloors ; i++) {
		if( dest[i] && i < min) return i; 
	}
	return min;
}

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

void
ElevatorManager::debugPrint() {
	int i;
	for(i = 0 ; i < numFloors ; i++ )
		printf("upwait[%d] = %d downwait[%d] = %d dest[%d] = %d\n" ,
			i, upwait[i] ,i , downwait[i] ,i, dest[i]);
	



}
int 
ElevatorManager::checkIfPeopleWaiting() {
	return upcount + downcount;

}
