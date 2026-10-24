// rott-c4ix.c -- Rise of the Triad on C4IX: the display is the desktop's,
// which gives it way until the game ends (include/display.h), or with
// --window the Terminal window it was started from; the clock is the
// machine's. gui.h for its declarations only: c4sdl.c carries its
// functions.

#include <stdint.h>
#define C4IX_SHARED 1
#include "gui.h"
#undef C4IX_SHARED
#include "display.h"
#include "window.h"
#include <stdlib.h>
#include <string.h>

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

// --window, or the display taken: the Terminal window the game was
// started from (window.h), as wolf3d --window plays. Two buffers: the
// desktop reads the one it was given when it next draws, and says so
// (WIN_FRAME); until then a newer frame waits in the other, and only the
// newest is sent. Keys come back in the display's own form, for c4sdl.c.
static char *fbuf[2];
static int *fpal[2];
static int fw, fh, sent, waiting, pending, closed;

int rott_win_open(int w, int h) {
	int i;
	fw = w; fh = h;
	for (i = 0; i < 2; i++) {
		fbuf[i] = (char *) calloc(w * h, 1);
		fpal[i] = (int *) calloc(256, sizeof(int));
		if (!fbuf[i] || !fpal[i]) return 0;
	}
	return win_open("Rise of the Triad");
}
static void send_frame(int b) {
	win_fb(fbuf[b], fw, fh, fw, fpal[b]);
	win_present();
	sent = b;
	waiting = 1;
}
void rott_win_draw(uint8_t *pixels, int w, int h, int pitch, uint32_t *palette) {
	int b, y;
	if (closed || w != fw || h != fh) return;
	b = waiting ? 1 - sent : sent;
	for (y = 0; y < h; y++) memcpy(fbuf[b] + y * w, pixels + y * pitch, w);
	memcpy(fpal[b], palette, 256 * sizeof(int));
	if (waiting) pending = 1;
	else send_frame(b);
}
int rott_win_poll(int *ev) {
	int w[3], k;
	if (closed) return 0;
	while (1) {
		k = win_poll(w);
		if (k == WIN_NONE) return 0;
		// the window closed: the game ends, as when it is quit
		if (k < 0) { closed = 1; exit(0); }
		if (k == WIN_FRAME) {
			waiting = 0;
			if (pending) { pending = 0; send_frame(1 - sent); }
			continue;
		}
		if (k == WIN_KEY || k == WIN_KEYUP) {
			ev[0] = k == WIN_KEY ? GUI_EV_KEYDOWN : GUI_EV_KEYUP;
			ev[1] = w[0]; ev[2] = w[1]; ev[3] = w[2];
			return 1;
		}
	}
}
void rott_win_close(void) { if (!closed) { win_close(); closed = 1; } }
uint32_t plat_ms(void) { return __time(); }
void plat_sleep(uint32_t ms) { if (ms) __c4_usleep(ms * 1000); }
