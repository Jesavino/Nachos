/* bogus2.c
 *
 * Data segment too big. Shouldn't even start to execute.
 *
 */

#include "syscall.h"

int big[100000];

int
main()
{

    Write("Shouldn't see this message!\n", 28, ConsoleOutput);

    Halt();
    /* not reached */
}
