GCCDIR = /home/f85/kearns/nachos_compiler/bin/
LDFLAGS = -T nachos.ld -n -nostartfiles
ASFLAGS =
CPPFLAGS = $(INCDIR) -DCHANGED


CC = $(GCCDIR)decstation-ultrix-gcc
AS = $(GCCDIR)decstation-ultrix-as
LD = $(GCCDIR)decstation-ultrix-ld

CPP = /usr/bin/cpp -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR) -DCHANGED -nostartfiles

all:	share sharekid noshare atomicfile atomicfile2

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

share.o: share.c
	$(CC) $(CFLAGS) -c share.c
share: share.o start.o
	$(LD) $(LDFLAGS) start.o share.o -o share.coff
	../bin/coff2noff share.coff share

sharekid.o: sharekid.c
	$(CC) $(CFLAGS) -c sharekid.c
sharekid: sharekid.o start.o
	$(LD) $(LDFLAGS) start.o sharekid.o -o sharekid.coff
	../bin/coff2noff sharekid.coff sharekid

noshare.o: noshare.c
	$(CC) $(CFLAGS) -c noshare.c
noshare: noshare.o start.o
	$(LD) $(LDFLAGS) start.o noshare.o -o noshare.coff
	../bin/coff2noff noshare.coff noshare

atomicfile.o: atomicfile.c
	$(CC) $(CFLAGS) -c atomicfile.c
atomicfile: atomicfile.o start.o
	$(LD) $(LDFLAGS) start.o atomicfile.o -o atomicfile.coff
	../bin/coff2noff atomicfile.coff atomicfile

atomicfile2.o: atomicfile2.c
	$(CC) $(CFLAGS) -c atomicfile2.c
atomicfile2: atomicfile2.o start.o
	$(LD) $(LDFLAGS) start.o atomicfile2.o -o atomicfile2.coff
	../bin/coff2noff atomicfile2.coff atomicfile2
