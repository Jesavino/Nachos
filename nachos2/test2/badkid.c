/* badkid.c
 *
 * Part of goodparent/badkid system.
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

    Exit(100);
    /* not reached */
}
