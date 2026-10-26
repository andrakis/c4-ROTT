// rott-bare.c -- Rise of the Triad on the bare machine and under C4DOS:
// the display is the game's to take, and there is no one to ask.
//
// Except under grin, C4DOS's graphical shell, which keeps the display and
// publishes a table in C4DOS's slot 25 while it does (c4's
// docs/grin-plan.md 3.6): then the game plays in a window of grin's, the
// window c4sdl.c asks for when the display is not the game's, the mouse
// locked to it with a click, and grin's loop runs in its polls.

#include <stdint.h>
#include <stdlib.h>
#define C4IX_SHARED 1
#include "gui.h"
#undef C4IX_SHARED

// grin's table (c4's src/grin/gkit.h): its first eight words
struct grin_table {
	int magic, version, gen;
	void *env;
	void (*hello)(void);
	int  (*fwin_open)(char *title, int w, int h, int flags);
	void (*fwin_frame)(char *pixels, int w, int h, int pitch, int *pal);
	int  (*fwin_poll)(int *ev);
};
extern int *__c4dos_api;                // bare.c's: C4DOS's table, 0 on the bare machine

static struct grin_table *grin(void) {
	struct grin_table *t;
	if (!__c4dos_api || __c4dos_api[0] != 4404292 || __c4dos_api[5] < 4) return 0;
	t = (struct grin_table *) __c4dos_api[25];
	return t && t->magic == 1196575054 && t->version >= 2 ? t : 0;
}

static struct grin_table *g;
static int fw, fh;

// one directory, the disk's: the game is where it starts
void rott_home(void) {}
int rott_open_display(void) { return !grin() && gui_present(); }
void rott_close_display(void) {}
uint32_t plat_ms(void) { return __time(); }
void plat_sleep(uint32_t ms) { if (ms) __c4_usleep(ms * 1000); }
// grin's window (flags 1: the mouse steers)
int rott_win_open(int w, int h) {
	fw = w; fh = h;
	return (g = grin()) != 0 && g->fwin_open("Rise of the Triad", w, h, 1);
}
void rott_win_draw(uint8_t *pixels, int w, int h, int pitch, uint32_t *palette) {
	if (g && w == fw && h == fh) g->fwin_frame((char *) pixels, w, h, pitch, (int *) palette);
}
// its next event, the mouse's in the display's 640x480 as c4sdl.c reads it
int rott_win_poll(int *ev) {
	int k;
	if (!g) return 0;
	// the window closed: the game ends, as when it is quit
	if ((k = g->fwin_poll(ev)) < 0) exit(0);
	if (k && (ev[0] == GUI_EV_MOVE || ev[0] == GUI_EV_DOWN || ev[0] == GUI_EV_UP || ev[0] == GUI_EV_REL)) {
		ev[1] = ev[1] * 640 / fw;
		ev[2] = ev[2] * 480 / fh;
	}
	return k;
}
void rott_win_close(void) {}
