// c4sdl.c -- c4/SDL.h's functions: SDL 1.2 as icculus.org's Rise of the
// Triad calls it, over libjs's display (libjs/guest/gui.h). The screen is
// one 8-bit surface the game draws in; SDL_UpdateRect sends it with the
// palette SDL_SetColors set, as GUI_BLIT8, which the host scales to the
// display's 640x480. A timedemo has no display (rott-sum.c) and the host's
// build none at all (rott-host.c); rott_frame sees every frame either way.
//
// This is the unit that carries gui.h's functions; the others take its
// declarations only.

#include "SDL.h"
#include "gui.h"

#include <stdio.h>

int  rott_open_display(void);      // rott-c4ix.c, rott-bare.c, rott-host.c
void rott_close_display(void);
int  rott_headless(void);          // rott-sum.c: a timedemo
int  rott_windowed(void);          //   and --window
int  rott_win_open(int w, int h);  // rott-*.c: a window instead (C4IX's)
void rott_win_draw(Uint8 *pixels, int w, int h, int pitch, Uint32 *palette);
int  rott_win_poll(int *ev);
void rott_win_close(void);
void rott_frame(Uint8 *pixels, int w, int h, int pitch, Uint32 *palette);
Uint32 rott_ticks(void);
void   rott_delay(Uint32 ms);

enum { W = 640, H = 480, RING = 65536 };

// ---- init ----
static Uint32 inited;
static char err[] = "not on this machine";
int    SDL_Init(Uint32 flags) { inited |= flags; return 0; }
int    SDL_InitSubSystem(Uint32 flags) { inited |= flags; return 0; }
void   SDL_QuitSubSystem(Uint32 flags) { inited &= ~flags; }
Uint32 SDL_WasInit(Uint32 flags) { return inited & flags; }
char  *SDL_GetError(void) { return err; }
void   SDL_ClearError(void) {}
static SDL_version linked = { 1, 2, 15 };
const SDL_version *SDL_Linked_Version(void) { return &linked; }

// ---- time ----
// The sound (rott-snd.c) is polled whenever the game reads the clock,
// which its waits do over and over, reads its input, which a menu
// waiting for a key does and nothing else, and with every frame
void rott_snd_poll(void);
Uint32 SDL_GetTicks(void) { rott_snd_poll(); return rott_ticks(); }
void   SDL_Delay(Uint32 ms) { rott_delay(ms); }

// ---- the display ----
static int on;
static int win;                    // in the Terminal's window instead (--window)
static int *ring;
static Uint32 palette[256];
static SDL_Surface *screen;
static SDL_Surface *small;         // the menus' 320x200, while shown (rott_show_small)
// the palette as the game set it, which it also reads back (VL_GetPalette)
static SDL_Color colors[256];
static SDL_Palette pal8 = { 256, colors };
static SDL_PixelFormat fmt8 = { &pal8, 8, 1 };
static SDL_VideoInfo info = { 0, 0, W, H };

static void close_display(void) {
	if (!on) return;
	gui_cursor(GUI_PTR_ARROW);
	gui_detach();
	rott_close_display();
	on = 0;
}
static void close_win(void) { if (win) rott_win_close(); win = 0; }
static int open_win(int w, int h) {
	if (!rott_win_open(w, h)) return 0;
	win = 1;
	atexit(close_win);
	return 1;
}
void SDL_Quit(void) { close_display(); close_win(); inited = 0; }

SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int w, int h, int depth, Uint32 rm, Uint32 gm, Uint32 bm, Uint32 am) {
	SDL_Surface *s;
	(void) flags; (void) depth; (void) rm; (void) gm; (void) bm; (void) am;
	s = (SDL_Surface *) calloc(1, sizeof *s);
	if (!s) return NULL;
	s->format = &fmt8;
	s->w = w; s->h = h; s->pitch = (Uint16) w;
	s->pixels = calloc(1, w * h);
	if (!s->pixels) { free(s); return NULL; }
	s->clip_rect.w = (Uint16) w; s->clip_rect.h = (Uint16) h;
	return s;
}
void SDL_FreeSurface(SDL_Surface *s) {
	if (!s) return;
	if (s == screen) screen = NULL;
	if (s == small) small = NULL;
	free(s->pixels);
	free(s);
}

SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, Uint32 flags) {
	(void) bpp;
	if (screen) SDL_FreeSurface(screen);
	screen = SDL_CreateRGBSurface(flags, w, h, 8, 0, 0, 0, 0);
	if (!screen) return NULL;
	if (on || win || rott_headless()) return screen;
	if (rott_windowed() && open_win(w, h)) return screen;
	if (!rott_open_display()) {
		if (open_win(w, h)) printf("rott: a program has the display; playing in the window\n");
		return screen;
	}
	{
		if ((ring = (int *) malloc(RING)) && gui_attach(ring, RING, W, H)) {
			gui_events(GUI_M_KEYS | GUI_M_MOVE | GUI_M_BUTTONS | GUI_M_WINDOW | GUI_M_REL);
			// the mouse turns the player: locked to the display from a
			// click on it (Esc lets it go), its motion as SDL's xrel
			gui_cursor(GUI_PTR_LOCK);
			gui_clear(0x000000);
			gui_show();
			on = 1;
			atexit(close_display);
		} else {
			rott_close_display();
		}
	}
	return screen;
}
SDL_Surface *SDL_GetVideoSurface(void) { return screen; }
const SDL_VideoInfo *SDL_GetVideoInfo(void) { return &info; }

// The screen again, as it is, in the palette just set: a VGA's palette
// shows at once, and SDL 1.2 draws an 8-bit screen again when its colours
// change. The game fades pictures in and out by the palette alone (the
// registered game's opening cinematic, DoInBetweenCinematic: drawn and
// flipped while the palette is black, then VL_FadeIn and a wait), and the
// display kept the black frame. Not a frame of the game's: rott_frame does
// not see it, so a timedemo's frames and checksum are as they were.
// The screen on the display: 640x480 for the sizes up to it (320x200 is
// the original's 4:3), and above that the display asked to be the
// screen's size (GUI_W, GUI_H), as Quake's resolutions are (Extended User
// Options' RESOLUTION). The picture sent, w by h of s, is scaled to it:
// the screen, or the menus' 320x200 (rott_show_small).
static int dw = W, dh = H;
static void to_display(SDL_Surface *s, int pw, int ph) {
	int sw = screen ? screen->w : s->w, sh = screen ? screen->h : s->h;
	int w = sw > W ? sw : W, h = sh > H ? sh : H;
	if (w != dw || h != dh) {
		dw = w; dh = h;
		*(int *) GUI_W = dw;
		*(int *) GUI_H = dh;
		gui_clear(0x000000);
	}
	gui_blit8(0, 0, pw, ph, (char *) s->pixels, s->pitch, (int *) palette, dw, dh);
	gui_show();
}
// the picture to the window (--window), whose desktop scales it too, or
// the display
static void present(SDL_Surface *s, int pw, int ph) {
	if (win) { rott_win_draw((Uint8 *) s->pixels, pw, ph, s->pitch, palette); return; }
	if (on) to_display(s, pw, ph);
}

// The menus and the game's other screens draw in 320x200 when the screen
// is bigger (modexlib.c's EnableScreenStretch, as the DOS game's were
// always 320x200), which SDL_SoftStretch then made the screen's size, 8 M
// instructions a frame at 640x480, most of a menu's time. Instead the
// 320x200 goes to the display (or window) as it is, and it scales it, as
// it does the 320x200 game. Not in a timedemo: rott_frame sees the
// stretched screen there, as before, so its frames and checksum stay as
// they were. small is that picture while it is the one shown, for a
// palette change to send again.
int rott_timing(void);              // rott-sum.c: a timedemo, shown or not
int rott_show_small(SDL_Surface *s) {
	if (!s || !s->pixels || rott_timing() || (!on && !win)) return 0;
	small = s;
	present(s, 320, 200);
	rott_snd_poll();
	return 1;
}

static void show_palette(void) {
	SDL_Surface *s = small ? small : screen;
	if (!s || !s->pixels) return;
	if (s == small) present(s, 320, 200);
	else present(s, s->w, s->h);
}

int SDL_SetColors(SDL_Surface *s, SDL_Color *c, int first, int n) {
	int i;
	(void) s;
	for (i = 0; i < n && first + i < 256; i++) {
		colors[first + i] = c[i];
		palette[first + i] = ((Uint32) c[i].r << 16) | ((Uint32) c[i].g << 8) | c[i].b;
	}
	show_palette();
	return 1;
}
int SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *c, int first, int n) {
	(void) flags;
	return SDL_SetColors(s, c, first, n);
}

// the whole screen, whatever rectangle is asked for: the game asks for all of it
void SDL_UpdateRect(SDL_Surface *s, Sint32 x, Sint32 y, Uint32 w, Uint32 h) {
	(void) x; (void) y; (void) w; (void) h;
	if (!s) return;
	rott_frame((Uint8 *) s->pixels, s->w, s->h, s->pitch, palette);
	small = NULL;
	if (!win && !on) return;
	present(s, s->w, s->h);
	rott_snd_poll();
}

// nearest-pixel, the only kind the game asks for (modexlib.c's stretch)
int SDL_SoftStretch(SDL_Surface *src, SDL_Rect *sr, SDL_Surface *dst, SDL_Rect *dr) {
	int x, y, sx, sy;
	Uint8 *d, *srow;
	for (y = 0; y < dr->h; y++) {
		sy = sr->y + y * sr->h / dr->h;
		srow = (Uint8 *) src->pixels + sy * src->pitch + sr->x;
		d = (Uint8 *) dst->pixels + (dr->y + y) * dst->pitch + dr->x;
		for (x = 0; x < dr->w; x++) {
			sx = x * sr->w / dr->w;
			d[x] = srow[sx];
		}
	}
	return 0;
}
SDL_bool SDL_SetClipRect(SDL_Surface *s, const SDL_Rect *r) {
	if (r) s->clip_rect = *r;
	else { s->clip_rect.x = s->clip_rect.y = 0; s->clip_rect.w = (Uint16) s->w; s->clip_rect.h = (Uint16) s->h; }
	return SDL_TRUE;
}
void SDL_GetClipRect(SDL_Surface *s, SDL_Rect *r) { *r = s->clip_rect; }

// ---- the window manager: there is none ----
SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode) { return mode == SDL_GRAB_QUERY ? SDL_GRAB_OFF : mode; }
int  SDL_WM_ToggleFullScreen(SDL_Surface *s) { (void) s; return 0; }
void SDL_WM_SetCaption(const char *title, const char *icon) { (void) title; (void) icon; }
int  SDL_ShowCursor(int toggle) { (void) toggle; return SDL_DISABLE; }

// ---- keys: a browser keyCode as SDL 1.2's keysym ----
static SDLKey keysym(int code) {
	if (code >= 'A' && code <= 'Z') return (SDLKey) (code + 32);
	if (code >= '0' && code <= '9') return (SDLKey) code;
	if (code >= 112 && code <= 123) return (SDLKey) (SDLK_F1 + code - 112);
	if (code >= 96 && code <= 105) return (SDLKey) (SDLK_KP0 + code - 96);
	switch (code) {
	case 13: return SDLK_RETURN;
	case 27: return SDLK_ESCAPE;
	case 8: return SDLK_BACKSPACE;
	case 9: return SDLK_TAB;
	case 32: return SDLK_SPACE;
	case 189: case 173: return SDLK_MINUS;
	case 187: case 61: return SDLK_EQUALS;
	case 219: return SDLK_LEFTBRACKET;
	case 221: return SDLK_RIGHTBRACKET;
	case 220: return SDLK_BACKSLASH;
	case 186: case 59: return SDLK_SEMICOLON;
	case 222: return SDLK_QUOTE;
	case 192: return SDLK_BACKQUOTE;
	case 188: return SDLK_COMMA;
	case 190: return SDLK_PERIOD;
	case 191: return SDLK_SLASH;
	case 20: return SDLK_CAPSLOCK;
	case 144: return SDLK_NUMLOCK;
	case 145: return SDLK_SCROLLOCK;
	case 19: return SDLK_PAUSE;
	case 45: return SDLK_INSERT;
	case 46: return SDLK_DELETE;
	case 36: return SDLK_HOME;
	case 35: return SDLK_END;
	case 33: return SDLK_PAGEUP;
	case 34: return SDLK_PAGEDOWN;
	case 37: return SDLK_LEFT;
	case 38: return SDLK_UP;
	case 39: return SDLK_RIGHT;
	case 40: return SDLK_DOWN;
	case 16: return SDLK_LSHIFT;
	case 17: return SDLK_LCTRL;
	case 18: return SDLK_LALT;
	case 106: return SDLK_KP_MULTIPLY;
	case 107: return SDLK_KP_PLUS;
	case 109: return SDLK_KP_MINUS;
	case 110: return SDLK_KP_PERIOD;
	case 111: return SDLK_KP_DIVIDE;
	}
	return SDLK_UNKNOWN;
}
static SDLMod keymod(int mods) {
	return (SDLMod) ((mods & 1 ? KMOD_LSHIFT : 0) | (mods & 2 ? KMOD_LCTRL : 0) | (mods & 4 ? KMOD_LALT : 0));
}

// ---- the event queue ----
enum { QLEN = 64 };
static SDL_Event q[QLEN];
static int qh, qt;
static int mx = W / 2, my = H / 2, mbuttons;
static SDL_EventFilter filter;

static void push(SDL_Event *e) {
	if (filter && !filter(e)) return;
	if ((qt + 1) % QLEN == qh) return;      // full: dropped
	q[qt] = *e;
	qt = (qt + 1) % QLEN;
}
void SDL_SetEventFilter(SDL_EventFilter f) { filter = f; }

// the display's events, as SDL's
static void gather(void) {
	int ev[4];
	SDL_Event e;
	while (win ? rott_win_poll(ev) : on && gui_poll(ev)) {
		memset(&e, 0, sizeof e);
		if (ev[0] == GUI_EV_KEYDOWN || ev[0] == GUI_EV_KEYUP) {
			e.type = ev[0] == GUI_EV_KEYDOWN ? SDL_KEYDOWN : SDL_KEYUP;
			e.key.state = ev[0] == GUI_EV_KEYDOWN ? SDL_PRESSED : SDL_RELEASED;
			e.key.keysym.sym = keysym(ev[1]);
			e.key.keysym.mod = keymod(ev[3]);
			e.key.keysym.unicode = (Uint16) (ev[2] > 0 && ev[2] < 127 ? ev[2] : 0);
			if (e.key.keysym.sym != SDLK_UNKNOWN) push(&e);
		} else if (ev[0] == GUI_EV_MOVE) {
			e.type = SDL_MOUSEMOTION;
			e.motion.x = (Uint16) ev[1]; e.motion.y = (Uint16) ev[2];
			e.motion.xrel = (Sint16) (ev[1] - mx); e.motion.yrel = (Sint16) (ev[2] - my);
			e.motion.state = (Uint8) mbuttons;
			mx = ev[1]; my = ev[2];
			push(&e);
		} else if (ev[0] == GUI_EV_REL) {
			e.type = SDL_MOUSEMOTION;
			mx += ev[1]; my += ev[2];
			if (mx < 0) mx = 0;
			if (mx > W - 1) mx = W - 1;
			if (my < 0) my = 0;
			if (my > H - 1) my = H - 1;
			e.motion.x = (Uint16) mx; e.motion.y = (Uint16) my;
			e.motion.xrel = (Sint16) ev[1]; e.motion.yrel = (Sint16) ev[2];
			e.motion.state = (Uint8) mbuttons;
			push(&e);
		} else if (ev[0] == GUI_EV_DOWN || ev[0] == GUI_EV_UP) {
			e.type = ev[0] == GUI_EV_DOWN ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
			e.button.button = ev[3] == 2 ? SDL_BUTTON_RIGHT : ev[3] == 1 ? SDL_BUTTON_MIDDLE : SDL_BUTTON_LEFT;
			e.button.state = ev[0] == GUI_EV_DOWN ? SDL_PRESSED : SDL_RELEASED;
			e.button.x = (Uint16) ev[1]; e.button.y = (Uint16) ev[2];
			if (ev[0] == GUI_EV_DOWN) mbuttons |= SDL_BUTTON(e.button.button);
			else mbuttons &= ~SDL_BUTTON(e.button.button);
			mx = ev[1]; my = ev[2];
			push(&e);
		}
	}
}

int SDL_PollEvent(SDL_Event *ev) {
	rott_snd_poll();          // a menu waiting for a key reads no clock
	gather();
	if (qh == qt) return 0;
	if (ev) {
		*ev = q[qh];
		qh = (qh + 1) % QLEN;
	}
	return 1;
}
Uint8 SDL_GetMouseState(int *x, int *y) {
	if (x) *x = mx;
	if (y) *y = my;
	return (Uint8) mbuttons;
}

// ---- joysticks: there are none ----
int  SDL_NumJoysticks(void) { return 0; }
SDL_Joystick *SDL_JoystickOpen(int i) { (void) i; return NULL; }
int  SDL_JoystickEventState(int state) { (void) state; return 0; }
int  SDL_JoystickNumButtons(SDL_Joystick *j) { (void) j; return 0; }
int  SDL_JoystickNumAxes(SDL_Joystick *j) { (void) j; return 0; }
Sint16 SDL_JoystickGetAxis(SDL_Joystick *j, int axis) { (void) j; (void) axis; return 0; }
Uint8 SDL_JoystickGetButton(SDL_Joystick *j, int button) { (void) j; (void) button; return 0; }
void SDL_JoystickClose(SDL_Joystick *j) { (void) j; }
const char *SDL_JoystickName(int i) { (void) i; return ""; }
