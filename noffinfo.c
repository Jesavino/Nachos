/***************************************************

   noffinfo: dump contents of noff binary
   Phil Kearns

   Just do a "gcc -m32 -o noffinfo noffinfo.c" to make the executable

   Use it as "./noffinfo some_noff_file"

***************************************************/


#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#define NOFFMAGIC       0xbadfad        /* magic number denoting Nachos 
                                         * object code file 
                                         */

typedef struct segment {
  int virtualAddr;              /* location of segment in virt addr space */
  int inFileAddr;               /* location of segment in this file */
  int size;                     /* size of segment */
} Segment;

typedef struct noffHeader {
   int noffMagic;               /* should be NOFFMAGIC */
   Segment code;                /* executable code segment */ 
   Segment initData;            /* initialized data segment */
   Segment uninitData;          /* uninitialized data segment --
                                 * should be zero'ed before use 
                                 */
} NoffHeader;

NoffHeader nhdr;

void pseg(int, int, int, int);

int main(int argc, char **argv)
{
  int fd, bytes;

  if (argc != 2) {
    fprintf(stderr, "usage: noffinfo noff_file_name\n");
    exit(1);
  }
  else
    printf("Info for NOFF file %s\n", argv[1]);

  if ((fd=open(argv[1],O_RDONLY)) < 0) {
    fprintf(stderr, "noffinfo: cannot open %s\n", argv[1]);
    exit(1);
  }

  if ((bytes = read(fd,&nhdr,sizeof(NoffHeader))) != sizeof(NoffHeader)) {
    fprintf(stderr, "noffinfo: only read %d of %d header bytes\n", bytes, (int) sizeof(NoffHeader));
    exit(1);
  }
  
  if (nhdr.noffMagic != NOFFMAGIC) {
    fprintf(stderr, "noffinfo: bad magic number\n");
    exit(1);
  }

  printf("\nCODE SEGMENT\n");
  if (nhdr.code.size == 0)
            printf("\t Does not exist (this is big trouble)\n");
  else {
    printf("\t starts at VA %d\n", nhdr.code.virtualAddr);
    printf("\t starts at file offset %d\n", nhdr.code.inFileAddr);
    printf("\t size is %d\n", nhdr.code.size);
    pseg(fd, nhdr.code.inFileAddr, nhdr.code.size, nhdr.code.virtualAddr);
  }
  printf("\nINITIALIZED DATA SEGMENT\n");
  if (nhdr.initData.size == 0)
	printf("\t Does not exist\n");
  else {
    printf("\t starts at VA %d\n", nhdr.initData.virtualAddr);
    printf("\t starts at file offset %d\n", nhdr.initData.inFileAddr);
    printf("\t size is %d\n", nhdr.initData.size);
    pseg(fd, nhdr.initData.inFileAddr, nhdr.initData.size, nhdr.initData.virtualAddr);
  }
  printf("\nUNINITIALIZED DATA SEGMENT\n");
  if (nhdr.uninitData.size == 0)
            printf("\t Does not exist\n");
  else {
    printf("\t starts at VA %d\n", nhdr.uninitData.virtualAddr);
    printf("\t size is %d\n", nhdr.uninitData.size);
  }
  return 0;
}

#define divRoundUp(n,s)    (((n) / (s)) + ((((n) % (s)) > 0) ? 1 : 0))

void pseg(int fd, int offset, int numbytes, int startVA)
{
  int numlines, left, i, j, bytesread;
  unsigned char line[10];

  numlines = divRoundUp(numbytes,10);
  left = numbytes - (numlines-1)*10;

  lseek(fd, (long) offset, SEEK_SET);
  for (i=0; i<numlines-1; i++) {
    for (j=0; j<10; j++) line[j] = (unsigned char)0;
    bytesread = read(fd, line, 10);
    printf("%5d: ", startVA + 10*i);
    for (j=0; j<bytesread; j++)
      printf("%2x ", line[j]);
    printf("     ");
    for (j=0; j<bytesread; j++)
      if (isprint(line[j])) printf("%c", line[j]);
      else printf(" ");
    printf("\n");
  }

  for (j=0; j<10; j++) line[j] = (unsigned char)0;
  bytesread = read(fd, line, 10);
  printf("%5d: ", startVA + 10*i);
  for (j=0; j<left; j++)
    printf("%2x ", line[j]);
  for (j=left; j<10; j++)
    printf("   ");
  printf("     ");
  for (j=0; j<left; j++)
    if (isprint(line[j])) printf("%c", line[j]);
    else printf(" ");
  printf("\n");

}
