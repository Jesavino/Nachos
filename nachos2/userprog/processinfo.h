#ifdef CHANGED
#ifndef __PROCESSINFO_H__
#define __PROCESSINFO_H__

#include "list.h"
#define  RUNNING  0
#define  ZOMBIE   1
#define  DONE     2

class ProcessInfo {
 public:

  ProcessInfo(SpaceId id, SpaceId parentid);
  void ProcessJoin(SpaceId childId);
  int GetStatus();

 private:
  SpaceId pid;
  SpaceId parentId;
  List childId;
  int status;
    
};




#endif
#endif
