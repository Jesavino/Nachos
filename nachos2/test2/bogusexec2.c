/* bogusexec2.c
 *
 * Attempt to Exec text file. Parent shouldn't die.
 *
 */

#include "syscall.h"

#define NULL (void *)0

int
main()
{

  SpaceId kid;
  int joinval, retval;

  prints("PARENT exists\n", ConsoleOutput);
  if ((kid = Exec("textfile", NULL, 0)) == -1) {
    prints("Exec failed! Good.\n", ConsoleOutput);
    Exit(1);
  }
  prints("Whoa ... PARENT after Exec; kid pid is ", ConsoleOutput);
  printd((int)kid, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("Bad PARENT about to Join kid\n", ConsoleOutput);
  joinval = Join(kid);
  prints("Bad PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);

  Exit(0);
  /* not reached */
}


/* Print an integer "n" on open file descriptor "file". */

printd(n,file)
int n;
OpenFileId file;

{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  char buffer[11];
  
  if (n < 0) {
    buffer[pos++] = '-';
    n = -n;
  }
  
  if (n == 0) {
    Write("0",1,file);
    return;
  }

  for (i=0; i<10; i++) {
    d = n / divisor; n = n % divisor;
    if (d == 0) {
      if (!zflag) buffer[pos++] =  (char) (d % 10) + '0';
    } else {
      zflag = 0;
      buffer[pos++] =  (char) (d % 10) + '0';
    }
    divisor = divisor/10;
  }
  Write(buffer,pos,file);
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
