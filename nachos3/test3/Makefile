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

all:	lazy1 lazy2

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

lazy1.o: lazy1.c
	$(CC) $(CFLAGS) -c lazy1.c
lazy1: lazy1.o start.o
	$(LD) $(LDFLAGS) start.o lazy1.o -o lazy1.coff
	../bin/coff2noff lazy1.coff lazy1

lazy2.o: lazy2.c
	$(CC) $(CFLAGS) -c lazy2.c
lazy2: lazy2.o start.o
	$(LD) $(LDFLAGS) start.o lazy2.o -o lazy2.coff
	../bin/coff2noff lazy2.coff lazy2
