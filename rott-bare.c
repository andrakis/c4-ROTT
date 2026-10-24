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
// no window: the display or nothing
int rott_win_open(int w, int h) { (void) w; (void) h; return 0; }
void rott_win_draw(uint8_t *pixels, int w, int h, int pitch, uint32_t *palette) { (void) pixels; (void) w; (void) h; (void) pitch; (void) palette; }
int rott_win_poll(int *ev) { (void) ev; return 0; }
void rott_win_close(void) {}
