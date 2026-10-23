// interrup.h -- audiolib turned interrupts off around what its timer task
// also touched. The task runs from rott-snd.c's poll here, never in the
// middle of anything, so there is nothing to turn off.
#ifndef __C4ROTT_INTERRUPT_H
#define __C4ROTT_INTERRUPT_H
#define DisableInterrupts() 0
#define RestoreInterrupts(flags) ((void) (flags))
#endif
