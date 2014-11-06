/* cp.c
 *	Simple program to copy a file to another file.
 *	Invoke as:
 *          cp [file to be copied] [file copied to]
 */

#include "syscall.h"

int main(argc, argv)
int argc;
char *argv[];
{
  OpenFileId input, output;
  char c;
  int numbytes;
  if (argc != 3) {
    Exit(1);
  }

  prints("here\n", ConsoleOutput);
  Create(argv[2]);
  output = Open(argv[2]);
  
  input = Open(argv[1]);
  if (input == -1 || output == -1) {
    Exit(2);
  }
  prints("here\n", ConsoleOutput);
  numbytes = 0;
  
  while (Read(&c, 1, input) == 1){
    numbytes++;
    Write(&c, 1, output);
  }
  prints("here\n", ConsoleOutput);
 
  Close(input);
  Close(output);
  
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
