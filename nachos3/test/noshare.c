/* noshare.c
 *
 * Parent in shared file test.
 * But this parent doesn't want to share its files.
 *
 */

#include "syscall.h"

int
main()
{

  SpaceId kid;
  int joinval, numbytes;
  char buffer[20], onechar;
  OpenFileId sharedfile;
  char *args[2];
  

  sharedfile = Open("data");
  prints("Output Open returned descriptor ", ConsoleOutput);
  printd((int)sharedfile, ConsoleOutput);
  prints("\n", ConsoleOutput);
  
  numbytes = Read(buffer, 20, sharedfile);
  prints("PARENT read ", ConsoleOutput);
  printd(numbytes, ConsoleOutput);
  prints(" bytes\n", ConsoleOutput);

  prints("Data from the read was: <", ConsoleOutput);
  Write(buffer, 20, ConsoleOutput);
  prints(">\n", ConsoleOutput);

  args[0] = "sharekid";
  args[1] = (char *) 0;

  if ((kid = Exec("sharekid", args, 0)) == -1) { 
    prints("Exec failed!, terminating\n", ConsoleOutput);
    Exit(1);
  }

  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("PARENT about to read a byte from shared file\n");

  numbytes = Read(&onechar, 1, sharedfile);
  prints("PARENT read ", ConsoleOutput);
  printd(numbytes, ConsoleOutput);
  prints(" bytes\n", ConsoleOutput);

  prints("Data from the read was: <", ConsoleOutput);
  Write(&onechar, 1, ConsoleOutput);
  prints(">\n", ConsoleOutput);

  Close(sharedfile);

  numbytes = Read(&onechar, 1, sharedfile);
  prints("PARENT read from closed file returned ", ConsoleOutput);
  printd(numbytes, ConsoleOutput);
  prints("\n", ConsoleOutput);

  Exit(0);
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
