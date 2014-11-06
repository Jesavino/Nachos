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

  if (argc == 1) {
    prints("cp: ", ConsoleOutput);
    prints(": Missing file operand\n", ConsoleOutput);
    
    Exit(1);
  }
  if (argc == 2) {
    prints("cp: ", ConsoleOutput);
    prints(": Missing destination file operand after ", ConsoleOutput);
    prints(argv[1], ConsoleOutput);
    prints("\n", ConsoleOutput);
 
    Exit(1);
  }

  Create(argv[2]);
  output = Open(argv[2]);
  
  input = Open(argv[1]);
  if (input == -1) {
    prints("cp: ", ConsoleOutput);
    prints(argv[1], ConsoleOutput);
    prints(": No such file or directory\n", ConsoleOutput);

    Exit(2);
  }
  if (input == -1) {
    prints("cp: ", ConsoleOutput);
    prints(argv[2], ConsoleOutput);
    prints(": No such file or directory\n", ConsoleOutput);

    Exit(2);
  }
  numbytes = 0;
  
  while (Read(&c, 1, input) == 1){
    numbytes++;
    Write(&c, 1, output);
  }
 
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
