// rott-c4ix.c -- Rise of the Triad on C4IX: the display is the desktop's,
// which gives it way until the game ends (include/display.h); the clock
// is the machine's. gui.h for its declarations only: c4sdl.c carries its
// functions.

#include <stdint.h>
#define C4IX_SHARED 1
#include "gui.h"
#undef C4IX_SHARED
#include "display.h"

// The game lives in /usr/games/rott, where its config and saved games go
// too, and it starts there; and it asks to be served sooner and longer
// than the programs beside it (nice, sched.c)
int unice(int id, int nice);            // libc4ix (c4ix_user.h)
int chdir(char *path);
void rott_home(void) {
	chdir("/usr/games/rott");
	unice(0, -5);
}

int rott_open_display(void) {
	if (!gui_present()) return 0;
	return udisplay_take(0);
}
void rott_close_display(void) { udisplay_give(); }
uint32_t plat_ms(void) { return __time(); }
void plat_sleep(uint32_t ms) { if (ms) __c4_usleep(ms * 1000); }
