#ifdef CHANGED
#ifndef __PROCESSINFO_H__
#define __PROCESSINFO_H__

#include "list.h"
#include "synch.h"
#include "thread.h"

// various stati? statuses? states.
#define  RUN  0
#define  ZOMBIE   1
#define  DONE     2

typedef int SpaceId;

class ProcessInfo {
 public:

  // CONSTRUCTORS
  ProcessInfo(SpaceId id, SpaceId parentid);
  ProcessInfo(SpaceId id);

  // DESTRUCTOR
  ~ProcessInfo();

  // other
  int ProcessJoin(SpaceId childId);
  int GetStatus();
  int GetExitStatus();
  void setStatus(int newStatus);
  void setExitStatus(int eStatus);
  void WakeParent();
  ProcessInfo * GetChild();

  int GetPid();
  void AddChild(ProcessInfo *child);

 private:

  SpaceId pid;
  SpaceId parentId;
  List *children;
  int status;
  int exitStatus;
  Lock * lock;
  Condition * cond;

};




#endif
#endif
