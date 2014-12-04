In order to get past the checkpoint, you must compile
and execute all of the following user-level code on top
of your kernel. It's probably easiest to copy the source
files into your test subdirectory and modify test/Makefile
in the obvious way to build the binaries for testing.

You should handle these test programs from top to bottom.
hellocons.c is a simple program which simply sends a string
to the console; fileio.c copies a file and dumps messages
on the console.

   File                 Tests
-----------     ------------------------------------
hellocons.c     Console output
fromcons.c      Console input and output
hellofile.c     Creates and writes into a file
fileio.c        Input and output with files.

