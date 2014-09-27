These are drop-in replacements for threads/threadtest.cc under Linux.

threadtest_perm.cc implements the permission-to-proceed control
     paradigm

threadtest_barrier_visual.cc implements a 20 thread system which
     has a synchronization barrier. It also has a little visual
     display to show that the barrier is working.

To build the permission-to-proceed system (from the Nachos top-level directory):

1) mv threads/threadtest.cc threads/threadtest.cc.orig
2) cp threadtest_perm.cc threads/threadtest.cc
3) make clean
4) make
5) Run the system with the command threads/nachos -rs X
   (where X is an int used to seed a random number generator that
   drives "random Yielding" of executing threads)

The other systems can be built similarly.

Look over the code for these examples carefully. In particular, note
how toggling interrupts off and on is used to advance time. The random
Yielding is done at randomly chosen points of time; the only way that
(simulated) time advances in Nachos #1 is through toggling the state of
the interrupt subsystem.
