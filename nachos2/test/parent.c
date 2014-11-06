/* parent.c
 *
 * Parent in simple parent/child system.
 *
 */

#include "syscall.h"

int
main()
{
	OpenFileId output, input;
  SpaceId kid;
  int joinval;
	int count = 0;
	char c;

  prints("PARENT exists\n", ConsoleOutput);
	Create("simple.output");
	output = Open("simple.output");
	input = Open("simple.input");

  while ((Read(&c, 1, input) == 1) && ( count < 5)){
      Write(&c, 1, output);
			count++;
  }
  kid = Exec("kid", (char **)0 ,1);
  prints("PARENT after exec; kid pid is ", ConsoleOutput);
  printd((int)kid, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("PARENT about to Join kid\n", ConsoleOutput);
  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", 1, ConsoleOutput);
	Close(input);
	Close(output);
	prints("PARENT closed both files\n", ConsoleOutput);
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

