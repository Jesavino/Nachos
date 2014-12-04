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

all:	parent kid bogusexec1 bogusexec2 bogusexec3 deepexec deepkid1 deepkid2\
	   hugekid maxexec goodparent badkid atomicconsole atomicconsole2 badparent

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

maxexec.o: maxexec.c
	$(CC) $(CFLAGS) -c maxexec.c
maxexec: maxexec.o start.o
	$(LD) $(LDFLAGS) start.o maxexec.o -o maxexec.coff
	../bin/coff2noff maxexec.coff maxexec

parent.o: parent.c
	$(CC) $(CFLAGS) -c parent.c
parent: parent.o start.o
	$(LD) $(LDFLAGS) start.o parent.o -o parent.coff
	../bin/coff2noff parent.coff parent

kid.o: kid.c
	$(CC) $(CFLAGS) -c kid.c
kid: kid.o start.o
	$(LD) $(LDFLAGS) start.o kid.o -o kid.coff
	../bin/coff2noff kid.coff kid

bogusexec1.o: bogusexec1.c
	$(CC) $(CFLAGS) -c bogusexec1.c
bogusexec1: bogusexec1.o start.o
	$(LD) $(LDFLAGS) start.o bogusexec1.o -o bogusexec1.coff
	../bin/coff2noff bogusexec1.coff bogusexec1

bogusexec2.o: bogusexec2.c
	$(CC) $(CFLAGS) -c bogusexec2.c
bogusexec2: bogusexec2.o start.o
	$(LD) $(LDFLAGS) start.o bogusexec2.o -o bogusexec2.coff
	../bin/coff2noff bogusexec2.coff bogusexec2

deepexec.o: deepexec.c
	$(CC) $(CFLAGS) -c deepexec.c
deepexec: deepexec.o start.o
	$(LD) $(LDFLAGS) start.o deepexec.o -o deepexec.coff
	../bin/coff2noff deepexec.coff deepexec

deepkid1.o: deepkid1.c
	$(CC) $(CFLAGS) -c deepkid1.c
deepkid1: deepkid1.o start.o
	$(LD) $(LDFLAGS) start.o deepkid1.o -o deepkid1.coff
	../bin/coff2noff deepkid1.coff deepkid1

deepkid2.o: deepkid2.c
	$(CC) $(CFLAGS) -c deepkid2.c
deepkid2: deepkid2.o start.o
	$(LD) $(LDFLAGS) start.o deepkid2.o -o deepkid2.coff
	../bin/coff2noff deepkid2.coff deepkid2

bogusexec3.o: bogusexec3.c
	$(CC) $(CFLAGS) -c bogusexec3.c
bogusexec3: bogusexec3.o start.o
	$(LD) $(LDFLAGS) start.o bogusexec3.o -o bogusexec3.coff
	../bin/coff2noff bogusexec3.coff bogusexec3

hugekid.o: hugekid.c
	$(CC) $(CFLAGS) -c hugekid.c
hugekid: hugekid.o start.o
	$(LD) $(LDFLAGS) start.o hugekid.o -o hugekid.coff
	../bin/coff2noff hugekid.coff hugekid

goodparent.o: goodparent.c
	$(CC) $(CFLAGS) -c goodparent.c
goodparent: goodparent.o start.o
	$(LD) $(LDFLAGS) start.o goodparent.o -o goodparent.coff
	../bin/coff2noff goodparent.coff goodparent

badkid.o: badkid.c
	$(CC) $(CFLAGS) -c badkid.c
badkid: badkid.o start.o
	$(LD) $(LDFLAGS) start.o badkid.o -o badkid.coff
	../bin/coff2noff badkid.coff badkid

atomicconsole.o: atomicconsole.c
	$(CC) $(CFLAGS) -c atomicconsole.c
atomicconsole: atomicconsole.o start.o
	$(LD) $(LDFLAGS) start.o atomicconsole.o -o atomicconsole.coff
	../bin/coff2noff atomicconsole.coff atomicconsole

atomicconsole2.o: atomicconsole2.c
	$(CC) $(CFLAGS) -c atomicconsole2.c
atomicconsole2: atomicconsole2.o start.o
	$(LD) $(LDFLAGS) start.o atomicconsole2.o -o atomicconsole2.coff
	../bin/coff2noff atomicconsole2.coff atomicconsole2

badparent.o: badparent.c
	$(CC) $(CFLAGS) -c badparent.c
badparent: badparent.o start.o
	$(LD) $(LDFLAGS) start.o badparent.o -o badparent.coff
	../bin/coff2noff badparent.coff badparent
