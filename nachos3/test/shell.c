#include "syscall.h"

int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[180];
    int i, j, spaceCount;
    char *argv[10];

    prompt[0] = '-';
    prompt[1] = '-';
    spaceCount = 0;

    while( 1 )
    {
	Write(prompt, 2, output);

	i = 0;
	j = 0;
	argv[j++] = &buffer[0];
	do {
	
	    Read(&buffer[i], 1, input); 
	    if (buffer[i] == ' ' && spaceCount == 0) {
	      spaceCount++;
	      buffer[i] = '\0';
	      argv[j++] = &buffer[i+1];
	    }
	    
	    else if (buffer[i] != ' ') {
	      spaceCount = 0;
	    }
	    else i--;

	} while( buffer[i++] != '\n' );
	
	buffer[--i] = '\0';
	argv[j] = (char *) 0;
	/*
	for (j = 0; argv[j] != (char *) 0; j++) {
	  prints(argv[j], output);
	  prints("\n", output);
	}
	*/
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
