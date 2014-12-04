#include "syscall.h"

int
main()
{

  SpaceId pid = Exec("hellocons", (char *) 0, 0);

  Join(pid);
  Exit(0);

}
