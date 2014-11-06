/* hellocons.c
 *	Simple program to echo input to console.
 *	Invoke as:
 *      cat [filename]
 */

#include "syscall.h"

int
main(argc, argv)
     int argc;
     char * argv[];
{
  OpenFileId input = Open(argv[1]);
  char ch;
  while (Read(&ch, 1, input) == 1){
    Write(&ch, 1, ConsoleOutput);
  }
  
  Exit(0);
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
