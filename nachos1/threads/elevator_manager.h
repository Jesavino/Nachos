// an implementation of the elevator

#include "copyright.h"
#include "thread.h"
#include "synch.h"
#include "limits.h"

class ElevatorManager {
  private:
	int pos, busy;
	int directionUp; // 0 is down, 1 is up;
	int upcount, downcount;
	int numFloors;
	int next;
	int upwait[5] , downwait[5];
	Condition *elevator;
	Condition *upsweep[5];
	Condition *downsweep[5];
	int dest[5];
	Lock *mutex;

	int find_nextUp(int position);
	int find_nextDown(int position);

	void debugPrint();
  public:
	ElevatorManager(Lock * eMutex , Condition * eCond );
	void ArrivingGoingFromTo(int aFloor, int toFloor , int id);
	void Travel();
	int checkIfPeopleWaiting();
};
