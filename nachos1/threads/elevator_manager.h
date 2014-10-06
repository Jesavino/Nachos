// an implementation of the elevator

#include "copyright.h"
#include "thread.h"
#include "synch.h"
#include "limits.h"
//#include <vector>

// -------------------------------------------------------------------------
// 
// This class defines the ElevatorManager. It is responsible for the general
// management of the elevator. It handles processing the requests for the 
// elevator, in addition to the actual movement of the elevator. 
//
// --------------------------------------------------------------------------

static const int numberOfFloors = 5;

class ElevatorManager {
  private:
	// state variables for position of the elevator and how many people are
	// waiting / where they are going. 
	int pos;
	int directionUp; // 0 is down, 1 is up;
	int upcount, downcount;
	int next;
	int upwait[numberOfFloors];
	int downwait[numberOfFloors];
	int dest[numberOfFloors]; 

	// the conditions the elevator and people will be waiting on. 
	Condition *elevator;
	Condition *downsweep[numberOfFloors];
	Condition *upsweep[numberOfFloors];
	Lock *mutex;

	// internal functions used to find the next person waiting to be picked
	// up or to be dropped off
	int find_nextUp(int position);
	int find_nextDown(int position);

	// used for internal debugging only
	void debugPrint();
  public:
	ElevatorManager(Lock * eMutex , Condition * eCond );

	// Handles requests for the elevator
	void ArrivingGoingFromTo(int aFloor, int toFloor , int id);

	// used to move the elevator
	void Travel();
	int checkIfPeopleWaiting();
};
