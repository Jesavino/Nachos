#include "processinfo.h"

ProcessInfo::ProcessInfo(SpaceId id, SpaceId parentid) {
  pid = id;
  parentId = parentid;
  status = RUN;
  cond = new(std::nothrow) Condition("process Condition");
  lock = new(std::nothrow) Lock("process Lock");
  children = new(std::nothrow) List();
  printf("in processinfo\n");
}

ProcessInfo::~ProcessInfo() {

  // delete all the processinfos in the list, then delete the list itself.
  delete children;
  delete cond;
  delete lock;
}

int ProcessInfo::ProcessJoin(SpaceId childId) {
  // wait until child is done running, then return its exit status
  // there should be a list of children. Find the correct child.
  // if its status == DONE, get its exitStatus and return that
  // if status != done, wait until it is.
  // when done, can delete the child.
  lock->Acquire();
  SpaceId first = -1;
  SpaceId last;
  ProcessInfo * child = (ProcessInfo *)children->Remove();
  
  while (first != last) {
    if (childId == child->GetPid()) {
      break;
    }
    children->Append(child);
    child = (ProcessInfo *) children->Remove();
    last = child->GetPid();
  }
  printf("about to join\n");
  child->lock->Acquire();
  if (child->GetStatus() != DONE) {
    child->cond->Wait(child->lock);
  }
  printf("joined\n");
  int eStatus = child->GetExitStatus();
  child->lock->Release();
  delete child;

  lock->Release();

  return eStatus;
}





SpaceId ProcessInfo::GetPid() {
  lock->Acquire();
  int returnPid = pid;
  lock->Release();
  return returnPid;
}

void ProcessInfo::AddChild(ProcessInfo *child) {
  lock->Acquire();
  children->Append(child);
  lock->Release();
}


int ProcessInfo::GetStatus() {
  //  lock->Acquire();
  int returnStatus = status;
  //lock->Release();
  return returnStatus;
}

int ProcessInfo::GetExitStatus() {
  //  lock->Acquire();
  int returnStatus = exitStatus;
  //lock->Release();
  return returnStatus;
}

void ProcessInfo::setExitStatus(int eStatus) {
  lock->Acquire();
  printf("in processinfo::setexitstatus\n");
  exitStatus = eStatus;
  lock->Release();
}

void ProcessInfo::setStatus(int newStatus) {
  lock->Acquire();
  status = newStatus;
  lock->Release();
}

void ProcessInfo::WakeParent() {
  lock->Acquire();
  cond->Signal(lock);
  lock->Release();
}
