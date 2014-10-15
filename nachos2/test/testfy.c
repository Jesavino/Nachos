#include "syscall.h"

void 
perAlarm()
{
  char * s = "forked\n";
  prints(s, ConsoleOutput);
  Yield();
}

int 
main() 
{  
  char *s = "Hello, console.\n";
  
  prints(s, ConsoleOutput);
  
  Fork(perAlarm);
  Fork(perAlarm);
  Fork(perAlarm);
  Fork(perAlarm);
  Fork(perAlarm);

  Halt();  
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


