/* widekid3.c
 *
 * Child in the wideexec system. Very slow terminator.
 *
 */

#include "syscall.h"

int
main()
{
  int i, j;

  /* Very slow process */
  j = 0  ;
  for (i=0; i<1000000; i++) j++;
  Exit(3);
  /* Should not get past here */
  prints("KID3 after Exit()!\n", ConsoleOutput);
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

