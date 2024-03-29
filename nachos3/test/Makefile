# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as
GCCDIR = /home/f85/kearns/nachos_compiler/bin/
#LDFLAGS = -T script -n -nostartfiles
LDFLAGS = -T nachos.ld -n -nostartfiles
ASFLAGS =
CPPFLAGS = $(INCDIR) -DCHANGED


# if you aren't cross-compiling:
#GCCDIR =
#LDFLAGS = -N -T 0
#ASFLAGS =
#CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)decstation-ultrix-gcc
AS = $(GCCDIR)decstation-ultrix-as
LD = $(GCCDIR)decstation-ultrix-ld

CPP = /usr/bin/cpp -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR) -DCHANGED -nostartfiles

all: halt shell matmult sort hellocons fromcons hellofile fileio testfy testexec bogus2\
	parent kid cat cp argtest argkid deepexec kid4 kid5 wideexec widekid1 widekid2\
	widekid3 bogus1 Xkid qmatmult qsort vmtorture

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

bogus1.o: bogus1.c
	$(CC) $(CFLAGS) -c bogus1.c
bogus1: bogus1.o start.o
	$(LD) $(LDFLAGS) start.o bogus1.o -o bogus1.coff
	../bin/coff2noff bogus1.coff bogus1

wideexec.o: wideexec.c
	$(CC) $(CFLAGS) -c wideexec.c
wideexec: wideexec.o start.o
	$(LD) $(LDFLAGS) start.o wideexec.o -o wideexec.coff
	../bin/coff2noff wideexec.coff wideexec

widekid1.o: widekid1.c
	$(CC) $(CFLAGS) -c widekid1.c
widekid1: widekid1.o start.o
	$(LD) $(LDFLAGS) start.o widekid1.o -o widekid1.coff
	../bin/coff2noff widekid1.coff widekid1

widekid2.o: widekid2.c
	$(CC) $(CFLAGS) -c widekid2.c
widekid2: widekid2.o start.o
	$(LD) $(LDFLAGS) start.o widekid2.o -o widekid2.coff
	../bin/coff2noff widekid2.coff widekid2

widekid3.o: widekid3.c
	$(CC) $(CFLAGS) -c widekid3.c
widekid3: widekid3.o start.o
	$(LD) $(LDFLAGS) start.o widekid3.o -o widekid3.coff
	../bin/coff2noff widekid3.coff widekid3

argkid.o: argkid.c
	$(CC) $(CFLAGS) -c argkid.c
argkid: argkid.o start.o
	$(LD) $(LDFLAGS) start.o argkid.o -o argkid.coff
	../bin/coff2noff argkid.coff argkid

deepexec.o: deepexec.c
	$(CC) $(CFLAGS) -c deepexec.c
deepexec: deepexec.o start.o
	$(LD) $(LDFLAGS) start.o deepexec.o -o deepexec.coff
	../bin/coff2noff deepexec.coff deepexec

kid4.o: kid4.c
	$(CC) $(CFLAGS) -c kid4.c
kid4: kid4.o start.o
	$(LD) $(LDFLAGS) start.o kid4.o -o kid4.coff
	../bin/coff2noff kid4.coff kid4

kid5.o: kid5.c
	$(CC) $(CFLAGS) -c kid5.c
kid5: kid5.o start.o
	$(LD) $(LDFLAGS) start.o kid5.o -o kid5.coff
	../bin/coff2noff kid5.coff kid5

argtest.o: argtest.c
	$(CC) $(CFLAGS) -c argtest.c
argtest: argtest.o start.o
	$(LD) $(LDFLAGS) start.o argtest.o -o argtest.coff
	../bin/coff2noff argtest.coff argtest

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

hellocons.o: hellocons.c
	$(CC) $(CFLAGS) -c hellocons.c
hellocons: hellocons.o start.o
	$(LD) $(LDFLAGS) start.o hellocons.o -o hellocons.coff
	../bin/coff2noff hellocons.coff hellocons

fromcons.o: fromcons.c
	$(CC) $(CFLAGS) -c fromcons.c
fromcons: fromcons.o start.o
	$(LD) $(LDFLAGS) start.o fromcons.o -o fromcons.coff
	../bin/coff2noff fromcons.coff fromcons

hellofile.o: hellofile.c
	$(CC) $(CFLAGS) -c hellofile.c
hellofile: hellofile.o start.o
	$(LD) $(LDFLAGS) start.o hellofile.o -o hellofile.coff
	../bin/coff2noff hellofile.coff hellofile

fileio.o: fileio.c
	$(CC) $(CFLAGS) -c fileio.c
fileio: fileio.o start.o
	$(LD) $(LDFLAGS) start.o fileio.o -o fileio.coff
	../bin/coff2noff fileio.coff fileio

testfy.o: testfy.c
	$(CC) $(CFLAGS) -c testfy.c
testfy: testfy.o start.o
	$(LD) $(LDFLAGS) start.o testfy.o -o testfy.coff
	../bin/coff2noff testfy.coff testfy

testexec.o: testexec.c
	$(CC) $(CFLAGS) -c testexec.c
testexec: testexec.o start.o
	$(LD) $(LDFLAGS) start.o testexec.o -o testexec.coff
	../bin/coff2noff testexec.coff testexec

bogus2.o: bogus2.c
	$(CC) $(CFLAGS) -c bogus2.c
bogus2: bogus2.o start.o
	$(LD) $(LDFLAGS) start.o bogus2.o -o bogus2.coff
	../bin/coff2noff bogus2.coff bogus2

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

cp.o: cp.c
	$(CC) $(CFLAGS) -c cp.c
cp: cp.o start.o
	$(LD) $(LDFLAGS) start.o cp.o -o cp.coff
	../bin/coff2noff cp.coff cp

cat.o: cat.c
	$(CC) $(CFLAGS) -c cat.c
cat: cat.o start.o
	$(LD) $(LDFLAGS) start.o cat.o -o cat.coff
	../bin/coff2noff cat.coff cat

Xkid.o: Xkid.c
	$(CC) $(CFLAGS) -c Xkid.c
Xkid: Xkid.o start.o
	$(LD) $(LDFLAGS) start.o Xkid.o -o Xkid.coff
	../bin/coff2noff Xkid.coff Xkid

qmatmult.o: qmatmult.c
	$(CC) $(CFLAGS) -c qmatmult.c
qmatmult: qmatmult.o start.o
	$(LD) $(LDFLAGS) start.o qmatmult.o -o qmatmult.coff
	../bin/coff2noff qmatmult.coff qmatmult

qsort.o: qsort.c
	$(CC) $(CFLAGS) -c qsort.c
qsort: qsort.o start.o
	$(LD) $(LDFLAGS) start.o qsort.o -o qsort.coff
	../bin/coff2noff qsort.coff qsort

vmtorture.o: vmtorture.c
	$(CC) $(CFLAGS) -c vmtorture.c
vmtorture: vmtorture.o start.o
	$(LD) $(LDFLAGS) start.o vmtorture.o -o vmtorture.coff
	../bin/coff2noff vmtorture.coff vmtorture

