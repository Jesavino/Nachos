/* widekid1.c
 *
 * Child in the wideexec system. Fast terminator.
 *
 */

#include "syscall.h"

int
main()
{
  /* Fast process */
  Exit(1);
  /* Should not get past here */
  prints("KID1 after Exit()!\n", ConsoleOutput);
  Halt();
    /* not reached */
}

/* Print a null-terminated string "s" on open file
   descriptor "file". */

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

