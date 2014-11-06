#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[180];
    int i, j;
    char *argv[10];

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	j = 0;
	argv[j++] = &buffer[0];
	do {
	
	    Read(&buffer[i], 1, input); 
	    if (buffer[i] == ' ') {
	      buffer[i] = '\0';
	      argv[j++] = &buffer[i+1];
	    }

	} while( buffer[i++] != '\n' );
	
	
	buffer[--i] = '\0';
	argv[j] = (char *) 0;
	/*	for (j = 0; argv[j] != (char *) 0; j++) {
	  prints(argv[j], output);
	  prints("\n", output);
	  }*/

	if( i > 0 ) {
	  if ((newProc = Exec(argv[0], argv, 0)) == -1) {
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
