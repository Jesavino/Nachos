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
/*
void
ElevatorManager::ArrivingGoingFromTo( int aFloor , int bFloor) {
	mutex->Acquire();
	if (busy){
		if ( ((pos < aFloor) && (pos < bFloor)) ) {
			upcount++;
			upwait[bFloor]++;
			upsweep[bFloor]->Wait(mutex);
		} else {
			downcount++;
			downwait[bFloor]++;
			downsweep[bFloor]->Wait(mutex);
		}
	}
	busy = 1;
	pos = bFloor;
	mutex->Release();

}
*/
void
ElevatorManager::ArrivingGoingFromTo(int aFloor , int bFloor) {
	DEBUG('t' , "Arriving in Elevator Manager");
	elevator->Signal(mutex);
	if ( (pos <= aFloor) && (aFloor < bFloor) ) {
		mutex->Acquire();
		upcount++;
		upwait[aFloor]++;
		DEBUG('t' , "About to Go to sleep on mutex");
		debugPrint();
		upsweep[aFloor]->Wait(mutex);
	} else {
		downcount++;
		downwait[aFloor]++;
		debugPrint();
		downsweep[aFloor]->Wait(mutex);
	}
	DEBUG('t' , "Woke Up Person From Mutex");
	mutex->Acquire();
	dest[bFloor]++;
	elevator->Signal(mutex);
	if(directionUp)
		upsweep[bFloor]->Wait(mutex);
	else
		downsweep[bFloor]->Wait(mutex);

}
void 
ElevatorManager::Travel() {
	mutex->Acquire();
	printf("Got Mutex in Travel()\n");
	debugPrint();
	if (directionUp) {
		printf("We see that Up: %d , Down: %d\n" , upcount , downcount);
		if (upcount) {
			printf("Going Up!\n");
			next = find_nextUp(pos);		
			pos = next;
			printf("Moving to %d\n" , pos);
			upcount -= dest[pos];
			upwait[pos] = 0;
			dest[pos] = 0;
			upsweep[pos]->Broadcast(mutex);
			elevator->Wait(mutex);
			mutex->Release();
			Travel();
		} else {
			directionUp = 0;
			next = find_nextDown(pos);
			pos = next;
			downcount -= dest[pos];
			downwait[pos] = 0;
			dest[pos] = 0;
			downsweep[pos]->Broadcast(mutex);
			mutex->Release();
			Travel();
		}
	}
	else if (downcount) {
		if (upcount) {
			directionUp = 0;
			next = find_nextUp(pos);
			pos = next;
			upcount -= dest[pos];
			upwait[pos] = 0;
			dest[pos] = 0;
			upsweep[pos]->Broadcast(mutex);
			mutex->Release();
			Travel();
		} else {
			next = find_nextDown(pos);
			pos = next; 
			downcount -= dest[pos];
			downwait[pos] = 0;
			dest[pos] = 0;
			downsweep[pos]->Broadcast(mutex);
			mutex->Release();
			Travel();
		}
	}
	elevator->Wait(mutex);		
	
}
/*
void
ElevatorManager::DepartingAt(int curFloor) {
	mutex->Acquire();
	busy = 0;
	if (directionUp) {
		if (upcount) {
			upcount--;
			next = find_nextUp(pos);
			upwait[next] = 0;
			upsweep[next]->Signal(mutex); // do I need to release the lock here?
		} else {
			directionUp = 0;
			downcount--;
			next = find_nextDown(pos);
			downwait[next] = 0;
			downsweep[next]->Signal(mutex);
		}
	}
	else { // going down
		if (downcount) {
			downcount--;
			next = find_nextDown(pos);
			downwait[next]--;
			downsweep[next]->Signal(mutex);
		} else {
			directionUp = 1;
			upcount--;
			next = find_nextUp(pos);
			upwait[next] = 0;
			upsweep[next]->Signal(mutex);
	
		}
	}
	mutex->Release();

}
*/
int
ElevatorManager::find_nextUp(int position) {
	int i,min;

	for(i = position ; i < numFloors ; i++) {
		if(upwait[i]) {
			 min = i;
			printf("Min is: %d\n" , min);
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
	int i,min;

	for (i = position  ; i >= 0; i--) {
		if(downwait[i]) {
			 min = i;
			break;
		}
	}
	for (i = position ; i >= 0; i--) {
		if(dest[i] && i > min) return i;
	}
	return min;
}

void
ElevatorManager::debugPrint() {
	int i;
	for(i = 0 ; i < numFloors ; i++ )
		printf("upwait[%d] = %d downwait[%d] = %d dest[%d] = %d\n" ,
			i, upwait[i] ,i , downwait[i] ,i, dest[i]);
	



}
