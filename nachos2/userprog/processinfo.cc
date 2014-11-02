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
  child->lock->Acquire();
  if (child->GetStatus() != DONE) {
    child->cond->Wait(child->lock);
  }

  int eStatus = child->GetExitStatus();
  child->lock->Release();
  delete child;


  return eStatus;
}

SpaceId ProcessInfo::GetPid() {
  return pid;
}

void ProcessInfo::AddChild(ProcessInfo *child) {
  children->Append(child);
}


int ProcessInfo::GetStatus() {
  return status;
}

int ProcessInfo::GetExitStatus() {
  return exitStatus;
}

void ProcessInfo::setExitStatus(int eStatus) {
  exitStatus = eStatus;
}

void ProcessInfo::setStatus(int newStatus) {
  status = newStatus;
}

void ProcessInfo::WakeParent() {
  cond->Signal(lock);
}
