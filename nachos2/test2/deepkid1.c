/* deepkid1.c
 *
 * First-level child in the deepexec system.
 *
 */

#include "syscall.h"

#define NULL (void *)0

int
main()
{
  int i, joinval, j;
  SpaceId kid;

  for (i=0; i++; i<1000000) j++;
  /* loop to delay kid initially */

  kid = Exec("deepkid2", NULL, 0);
  prints("DEEPKID1 after exec; DEEPKID2 pid is ", ConsoleOutput);
  printd((int)kid, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("DEEPKID1 about to Join DEEPKID2\n", ConsoleOutput);
  joinval = Join(kid);
  prints("DEEPKID1 off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);

  Exit(4);
  /* Should not get past here */
  prints("DEEPKID1 after Exit()! Dammit!\n", ConsoleOutput);
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

