/* kid5.c
 *
 * Second-level child in the deepexec system.
 *
 */

#include "syscall.h"

int
main()
{
  int i, j;
  
  for (i=0; i++; i<100000) j++;
  /* loop to delay kid initially */
  Exit(5);
  /* Should not get past here */
  Write("DEEPKID2 after Exit()! Dammit!\n", 32, ConsoleOutput);
  Halt();
    /* not reached */
}
