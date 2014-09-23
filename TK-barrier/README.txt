These files will allow you to build a version of nachos which implements
barrier synchronization for 20 threads, and which has a tk-based GUI to
show the state of the system.

Put
  threadtest.cc
  child.h
  child.c
  barrier.tcl
into the threads subdirectory.

Put Makefile.common in the nachos top-level directory.

Do a make, cd to threads, run ./nachos -rs 10.

Crude, but visual ... and it took about an hour of hacking.
