/* lazy1.c
 *
 * Iterate over large address space. All data
 * pages get hit.
 *
 */

#include "syscall.h"

#define NULL (void *)0

#define ASIZE 10000

int A[ASIZE];

int main()
{
  int i, j;

  prints("Starting lazy1\n", ConsoleOutput);

  /* Iterate over all data pages */

  for  (i = 0; i < ASIZE; i++)
    A[i] = i;

  prints("lazy1 completed\n", ConsoleOutput);
  Halt();
}

/* Print a null-terminated string "s" on open file
   descriptor "file". */

prints(s,file)
char *s;
OpenFileId file;
{
  while (*s != '\0') {
    Write(s,1,file);
    s++;
  }
}

