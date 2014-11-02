#include "processinfo.h"

ProcessInfo::ProcessInfo(SpaceId id, SpaceId parentid) {
  pid = id;
  parentId = parentid;
  status = RUNNING;
}

void ProcessInfo::ProcessJoin(SpaceId childId) {
  
}

int ProcessInfo::GetStatus() {
  return status;
}
