// dos.h -- audiolib includes it for DOS's interrupt calls, which the parts
// of it here (midi.c, al_midi.c) do not make. Both include it first, so
// Watcom's calling-convention keyword goes here, as rt_def.h has it for the
// game.
#ifndef cdecl
#define cdecl
#endif
// and Watcom's min and max, as rt_def.h has them for the game
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
