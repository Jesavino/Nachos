#ifdef CHANGED
#ifndef __PROCESSINFO_H__
#define __PROCESSINFO_H__

#include "list.h"
#define  RUNNING  0
#define  ZOMBIE   1
#define  DONE     2

typedef int SpaceId;

class ProcessInfo {
 public:

  ProcessInfo(SpaceId id, SpaceId parentid);
  void ProcessJoin(SpaceId childId);
  int GetStatus();

 private:
  SpaceId pid;
  SpaceId parentId;
  List children;
  int status;
  int exitStatus;
};




#endif
#endif
