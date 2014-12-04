#include "syscall.h"

void
SimpleThread(i)
     int i;
{
    int num;
    prints("one\n", 1);
    Yield();
    prints("two\n", 1);
    Yield();
    prints("three\n", 1);
    Yield();
    prints("four\n", 1);
    Yield();
    prints("five\n", 1);
    Yield();

}


void
main()
{
  prints("start ping-pong\n", 1);
  Fork(SimpleThread);
  SimpleThread();
}




/* Print a null-terminated string "s" on open file descriptor "file". */

prints(s,file)
char *s;
OpenFileId file;

{
  int count = 0;
  char *p;

  p = s;
  while (*p++ != '\0') count++;
  Write(s, count, file);  

}


