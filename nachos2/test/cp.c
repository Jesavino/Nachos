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
  else if (argc == 2) {
    prints("cp: ", ConsoleOutput);
    prints(": Missing destination file operand after ", ConsoleOutput);
    prints(argv[1], ConsoleOutput);
    prints("\n", ConsoleOutput);
 
    Exit(1);
  }
  else if (argc > 3) {
    prints("cp: ", ConsoleOutput);
    prints(": Too many arguments\n", ConsoleOutput);

  }
  if (strcmp(argv[1], argv[2]) == 0) {
    prints("cp: '", ConsoleOutput);
    prints(argv[1], ConsoleOutput);
    prints("' and '", ConsoleOutput);
    prints(argv[2], ConsoleOutput);
    prints("' are the same file\n", ConsoleOutput);
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

/* check to see if two strings have all the same letters*/
/* returns 0 if they have all the same letters*/

int
strcmp(arg1, arg2)
char *arg1;
char *arg2;
{
  int size1, size2;
  int i;
  size1 = 0;
  size2 = 0;
  while (arg1[size1] != '\0') {
    size1++;
  }
  while (arg2[size2] != '\0') {
    size2++;
  }
  if (size1 != size2) return 1;

  for (i = 0; i < size1; i++) {
    if (arg1[i] != arg2[i]) return 1;
  }
  return 0;

}
