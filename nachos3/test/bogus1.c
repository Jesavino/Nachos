/* bogus1.c
 *
 * Make absurd array reference.
 *
 */

#include "syscall.h"

int
main()
{
    int arr[10];

    arr[10000] = 1;
    Write("Shouldn't see this message!\n", 28, ConsoleOutput);

    Halt();
    /* not reached */
}
