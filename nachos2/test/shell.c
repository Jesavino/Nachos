#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i, j;
    char *argv[10];

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	j = 0;
	/*	do {
	
	    Read(&buffer[i], 1, input); 

	} while( buffer[i++] != '\n' );
	*/
	
	do {
	  Read(&argv[i][j], 1, input);
	  if (argv[i][j] == ' ') {
	    argv[i++][j] == '\0';
	    prints(&argv[i-1], output);
	    j = 0;
	  }
	  
	} while (argv[i][j++] != '\n');
	argv[i++][--j] = '\0';
	&argv[i] = (char *) 0;

	if( i > 0 ) {
	  if ((newProc = Exec(&argv[0], &argv, 0)) == -1) {
	    prints("Cannot Exec file\n", ConsoleOutput);
	  }
	  else {
	    Join(newProc);
	  }
	}
    }
}


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
