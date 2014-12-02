/* lazy2.c
 *
 * Big address space. Very little used.
 * Should exhibit far less disk i/o
 * than lazy1.
 *
 */

#include "syscall.h"

#define NULL (void *)0

#define ASIZE 10000

int A[ASIZE];

int main()
{
  int i, j;

  prints("Starting lazy2\n", ConsoleOutput);

  j = 0;
  for  (i = 0; i < ASIZE; i++)
    j++;

  prints("lazy2 completed\n", ConsoleOutput);
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

