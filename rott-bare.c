// rott-bare.c -- Rise of the Triad on the bare machine and under C4DOS:
// the display is the game's to take, and there is no one to ask

#include <stdint.h>
#define C4IX_SHARED 1
#include "gui.h"
#undef C4IX_SHARED

// one directory, the disk's: the game is where it starts
void rott_home(void) {}
int rott_open_display(void) { return gui_present(); }
void rott_close_display(void) {}
uint32_t plat_ms(void) { return __time(); }
void plat_sleep(uint32_t ms) { if (ms) __c4_usleep(ms * 1000); }
