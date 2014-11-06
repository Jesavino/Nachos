/* argtest.c
 *
 * Parent in simple argument test. Note that this
 * assumes the three-argument Exec(). The sharing
 * flag is set to zero.
 *
 */

#include "syscall.h"

int
main()
{

  SpaceId kid;
  int joinval;
  char *args[3];

  args[0] = "argkid";
  args[1] = "argument1";
  args[2] = (char *)0;

  prints("PARENT exists\n", ConsoleOutput);
  kid = Exec("argkid",args,0); /*removed a 0 from the third arg here to test*/
  prints("PARENT after fork/exec; argkid pid is ", ConsoleOutput);
  printd((int)kid, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("PARENT about to Join argkid\n", ConsoleOutput);
  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
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
