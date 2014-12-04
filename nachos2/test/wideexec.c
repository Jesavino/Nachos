/* wideexec.c
 *
 * Parent creates widekid1, widekid2, and widekid3.
 * Should run smoothly.
 *
 * Change Exec("...") to Exec("...",NULL,0) to
 * compile and run after bulking up Exec() to
 * handle args and inherited files.
 *
 */

#include "syscall.h"

int
main()
{

  SpaceId kid1, kid2, kid3;
  int joinval1, joinval2, joinval3;

  prints("WIDEEXEC exists\n", ConsoleOutput);

  kid1 = Exec("widekid1", (char **)0 , 0);
  prints("WIDEEXEC after 1st exec; kid1 pid is ", ConsoleOutput);
  printd((int)kid1, ConsoleOutput);
  prints("\n", ConsoleOutput);

  kid2 = Exec("widekid2", (char **)0 , 0);
  prints("WIDEEXEC after 2nd exec; kid2 pid is ", ConsoleOutput);
  printd((int)kid2, ConsoleOutput);
  prints("\n", ConsoleOutput);

  kid3 = Exec("widekid3", (char **)0, 0);
  prints("WIDEEXEC after 3rd exec; kid3 pid is ", ConsoleOutput);
  printd((int)kid3, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("WIDEEXEC about to Join kid3\n", ConsoleOutput);
  joinval3 = Join(kid3);
  prints("WIDEEXEC off Join with value of ", ConsoleOutput);
  printd(joinval3, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("WIDEEXEC about to Join kid2\n", ConsoleOutput);
  joinval2 = Join(kid2);
  prints("WIDEEXEC off Join with value of ", ConsoleOutput);
  printd(joinval2, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("WIDEEXEC about to Join kid1\n", ConsoleOutput);
  joinval1 = Join(kid1);
  prints("WIDEEXEC off Join with value of ", ConsoleOutput);
  printd(joinval1, ConsoleOutput);
  prints("\n", ConsoleOutput);

  Exit(0);
  Halt();
  /* not reached */
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

