#ifdef CHANGED
#include "processinfo.h"

//----------------------------------------------------------------------
// ProcessInfo( id, parentid)
//     constructor for ProcessInfo
//     id is the process id of this process
//     parentid is the processid of the parent
//----------------------------------------------------------------------

ProcessInfo::ProcessInfo(SpaceId id, SpaceId parentid) {
  pid = id;
  parentId = parentid;
  status = RUN;
  cond = new(std::nothrow) Condition("process Condition");
  lock = new(std::nothrow) Lock("process Lock");
  children = new(std::nothrow) List();
  //printf("in processinfo\n");
}

//----------------------------------------------------------------------
// ~ProcessInfo()
//     Destructor
//     gets rid of any dynamically allocated data
//----------------------------------------------------------------------

ProcessInfo::~ProcessInfo() {

  // getting rid of the children is already taken care of in a system call.
  // terrible design, honestly. 
  delete children;
  delete cond;
  delete lock;
}

//----------------------------------------------------------------------
// ProcessJoin(childid)
//    wait until child is done running, then return its exit status
//    there should be a list of children. Find the correct child.
//    if its status == DONE, get its exitStatus and return that
//    if status != done, wait until it is.
//    when done, can delete the child.
//    childid is the pid of the child.
//----------------------------------------------------------------------

int ProcessInfo::ProcessJoin(SpaceId childId) {

  // get mutex on this processInfo, any parent will have to wait
  lock->Acquire();

  SpaceId first = -1;
  SpaceId last = -1;
  
  // get first child off the list
  ProcessInfo * child = (ProcessInfo *)children->Remove();
  if (child == NULL) {
    lock->Release();
    return -1;
  }

  // if we access the same child twice, the one we are looking for is 
  // not in the list
  first = child->GetPid();

  while (first != last) {
    // We found the correct child
    if (childId == child->GetPid()) {
      break;
    }

    children->Append(child);
    child = (ProcessInfo *) children->Remove();
    last = child->GetPid();
  }
  
  //if there is no child with that id
  if (childId != child->GetPid()) {
    // release mutex
    lock->Release();
    // put this child back in the list of children
    AddChild(child);
    return -1;
  }
  
  // get mutex on the child, will wait on that mutex.
  child->lock->Acquire();
  if (child->GetStatus() != DONE) {
    child->cond->Wait(child->lock);
  }

  int eStatus = child->GetExitStatus();
  child->lock->Release();
  delete child;

  lock->Release();

  return eStatus;
}

//----------------------------------------------------------------------
// GetPid()
//     return pid of this process
//----------------------------------------------------------------------

SpaceId ProcessInfo::GetPid() {

  int returnPid = pid;

  return returnPid;
}

//----------------------------------------------------------------------
// GetChild()
//     returns the processInfo of the first child in the list
//----------------------------------------------------------------------

ProcessInfo * ProcessInfo::GetChild() {
  return (ProcessInfo *) children->Remove();
}

//----------------------------------------------------------------------
// AddChild(child)
//     put child on the list of children
//----------------------------------------------------------------------

void ProcessInfo::AddChild(ProcessInfo *child) {

  lock->Acquire();

  children->Append(child);

  lock->Release();
}

//----------------------------------------------------------------------
// GetStatus()
//     returns the status of this process.
//----------------------------------------------------------------------

int ProcessInfo::GetStatus() {

  return status;

}

//----------------------------------------------------------------------
// GetExitStatus()
//      returns the exit status of this process
//----------------------------------------------------------------------

int ProcessInfo::GetExitStatus() {

  return exitStatus;

}

//----------------------------------------------------------------------
// setExitStatus(eStatus)
//     sets the exit status of this process
//----------------------------------------------------------------------

void ProcessInfo::setExitStatus(int eStatus) {
  lock->Acquire();
  exitStatus = eStatus;
  lock->Release();
}

//----------------------------------------------------------------------
// setStatus(newStatus(
//     sets the status of the process
//----------------------------------------------------------------------

void ProcessInfo::setStatus(int newStatus) {
  lock->Acquire();
  status = newStatus;
  lock->Release();
}

//----------------------------------------------------------------------
// WakeParent()
//     wake a parent that is waiting on this child.
//----------------------------------------------------------------------

void ProcessInfo::WakeParent() {
  lock->Acquire();
  cond->Signal(lock);
  lock->Release();
}



#endif
