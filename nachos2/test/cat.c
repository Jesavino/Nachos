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
  OpenFileId input;
  int i;
  char ch;
  
  if (argc == 1) {
      prints("cat: ", ConsoleOutput);
      prints(": No arguments\n", ConsoleOutput);
      Exit(1);
  }
  for (i = 1; i < argc; i++) {
    input = Open(argv[i]);
    if (input == -1) {
      prints("cat: ", ConsoleOutput);
      prints(argv[i], ConsoleOutput);
      prints(": No such file or directory\n", ConsoleOutput);
      Exit(1);
    }

    
    while (Read(&ch, 1, input) == 1){
      Write(&ch, 1, ConsoleOutput);
    }
    Close(input);
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
