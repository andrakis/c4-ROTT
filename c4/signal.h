// signal.h -- what Rise of the Triad asks of it: rt_main.c installs a
// handler for a segmentation fault, to print where it was. A c4 machine
// stops a program that strays with a trap of its own, and has no
// signals to deliver to it, so the handler is never called.
#ifndef C4ROTT_SIGNAL_H
#define C4ROTT_SIGNAL_H 1
#define SIGSEGV 11
#define signal(sig, handler) ((void) (handler), 0)
#endif
