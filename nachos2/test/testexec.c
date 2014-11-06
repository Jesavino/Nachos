#include "syscall.h"

int
main()
{

  SpaceId pid = Exec("test/hellocons", (char *)0, 0);

}
