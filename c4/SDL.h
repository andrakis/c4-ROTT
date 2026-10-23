// SDL.h -- the part of SDL 1.2 icculus.org's Rise of the Triad uses, and
// no more, over libjs's display (c4sdl.c). The game draws its 320x200
// 8-bit screen into the surface SDL_SetVideoMode gives it, and
// SDL_UpdateRect sends that to the display with the palette. The rest is
// the event queue, the clock, and the window manager calls of a desktop
// port, which have nothing to do here. Sound is not SDL_mixer's: the game's
// own FX_ and MUSIC_ interfaces are on libjs's sound card (rott-snd.c).
// Built by c4lc for C4IX, C4DOS and the bare machine, and by gcc for the
// host, the reference make test-rott holds them to.

#ifndef C4ROTT_SDL_H
#define C4ROTT_SDL_H 1

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t  Uint8;
typedef int8_t   Sint8;
typedef uint16_t Uint16;
typedef int16_t  Sint16;
typedef uint32_t Uint32;
typedef int32_t  Sint32;
typedef enum { SDL_FALSE = 0, SDL_TRUE = 1 } SDL_bool;

#define SDL_LIL_ENDIAN 1234
#define SDL_BIG_ENDIAN 4321
#define SDL_BYTEORDER SDL_LIL_ENDIAN

// ---- init ----
#define SDL_INIT_TIMER    0x00000001
#define SDL_INIT_AUDIO    0x00000010
#define SDL_INIT_VIDEO    0x00000020
#define SDL_INIT_JOYSTICK 0x00000200
int    SDL_Init(Uint32 flags);
int    SDL_InitSubSystem(Uint32 flags);
void   SDL_QuitSubSystem(Uint32 flags);
Uint32 SDL_WasInit(Uint32 flags);
void   SDL_Quit(void);
char  *SDL_GetError(void);
void   SDL_ClearError(void);

typedef struct { Uint8 major, minor, patch; } SDL_version;
#define SDL_VERSION(v) ((v)->major = 1, (v)->minor = 2, (v)->patch = 15)
const SDL_version *SDL_Linked_Version(void);

// ---- time ----
Uint32 SDL_GetTicks(void);
void   SDL_Delay(Uint32 ms);

// ---- video: 8 bits a pixel, which is all the game makes ----
typedef struct { Uint8 r, g, b, unused; } SDL_Color;
typedef struct { Sint16 x, y; Uint16 w, h; } SDL_Rect;
typedef struct { int ncolors; SDL_Color *colors; } SDL_Palette;
typedef struct { SDL_Palette *palette; Uint8 BitsPerPixel; Uint8 BytesPerPixel; } SDL_PixelFormat;
typedef struct {
	Uint32 flags;
	SDL_PixelFormat *format;
	int w, h;
	Uint16 pitch;
	void *pixels;
	SDL_Rect clip_rect;
} SDL_Surface;
typedef struct { Uint32 hw_available, wm_available; int current_w, current_h; } SDL_VideoInfo;
#define SDL_SWSURFACE  0x00000000
#define SDL_HWPALETTE  0x20000000
#define SDL_FULLSCREEN 0x80000000
#define SDL_OPENGL     0x00000002
#define SDL_OPENGLBLIT 0x0000000A
#define SDL_LOGPAL  0x01
#define SDL_PHYSPAL 0x02
SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, Uint32 flags);
SDL_Surface *SDL_GetVideoSurface(void);
const SDL_VideoInfo *SDL_GetVideoInfo(void);
SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int w, int h, int depth, Uint32 rm, Uint32 gm, Uint32 bm, Uint32 am);
void SDL_FreeSurface(SDL_Surface *s);
int  SDL_SetColors(SDL_Surface *s, SDL_Color *colors, int first, int n);
int  SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int first, int n);
void SDL_UpdateRect(SDL_Surface *s, Sint32 x, Sint32 y, Uint32 w, Uint32 h);
int  SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srect, SDL_Surface *dst, SDL_Rect *drect);
SDL_bool SDL_SetClipRect(SDL_Surface *s, const SDL_Rect *r);
void SDL_GetClipRect(SDL_Surface *s, SDL_Rect *r);
#define SDL_MUSTLOCK(s) 0
#define SDL_LockSurface(s) 0
#define SDL_UnlockSurface(s)

// ---- the window manager: nothing here ----
typedef enum { SDL_GRAB_QUERY = -1, SDL_GRAB_OFF = 0, SDL_GRAB_ON = 1 } SDL_GrabMode;
SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode);
int  SDL_WM_ToggleFullScreen(SDL_Surface *s);
void SDL_WM_SetCaption(const char *title, const char *icon);
#define SDL_QUERY   -1
#define SDL_DISABLE  0
#define SDL_ENABLE   1
int  SDL_ShowCursor(int toggle);

// ---- keys: SDL 1.2's keysyms ----
typedef enum {
	SDLK_UNKNOWN = 0, SDLK_BACKSPACE = 8, SDLK_TAB = 9, SDLK_RETURN = 13, SDLK_PAUSE = 19,
	SDLK_ESCAPE = 27, SDLK_SPACE = 32, SDLK_QUOTE = 39, SDLK_COMMA = 44, SDLK_MINUS = 45,
	SDLK_PERIOD = 46, SDLK_SLASH = 47,
	SDLK_0 = 48, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
	SDLK_SEMICOLON = 59, SDLK_EQUALS = 61, SDLK_LEFTBRACKET = 91, SDLK_BACKSLASH = 92,
	SDLK_RIGHTBRACKET = 93, SDLK_BACKQUOTE = 96,
	SDLK_a = 97, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i, SDLK_j,
	SDLK_k, SDLK_l, SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r, SDLK_s, SDLK_t,
	SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y, SDLK_z,
	SDLK_DELETE = 127, SDLK_WORLD_63 = 223,
	SDLK_KP0 = 256, SDLK_KP1, SDLK_KP2, SDLK_KP3, SDLK_KP4, SDLK_KP5, SDLK_KP6, SDLK_KP7,
	SDLK_KP8, SDLK_KP9, SDLK_KP_PERIOD, SDLK_KP_DIVIDE, SDLK_KP_MULTIPLY, SDLK_KP_MINUS,
	SDLK_KP_PLUS, SDLK_KP_ENTER, SDLK_KP_EQUALS,
	SDLK_UP = 273, SDLK_DOWN, SDLK_RIGHT, SDLK_LEFT, SDLK_INSERT, SDLK_HOME, SDLK_END,
	SDLK_PAGEUP, SDLK_PAGEDOWN,
	SDLK_F1 = 282, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6, SDLK_F7, SDLK_F8, SDLK_F9,
	SDLK_F10, SDLK_F11, SDLK_F12,
	SDLK_NUMLOCK = 300, SDLK_CAPSLOCK, SDLK_SCROLLOCK, SDLK_RSHIFT, SDLK_LSHIFT, SDLK_RCTRL,
	SDLK_LCTRL, SDLK_RALT, SDLK_LALT, SDLK_RMETA, SDLK_LMETA, SDLK_LSUPER, SDLK_RSUPER,
	SDLK_MODE,
	SDLK_LAST = 323
} SDLKey;
typedef enum {
	KMOD_NONE = 0, KMOD_LSHIFT = 0x1, KMOD_RSHIFT = 0x2, KMOD_LCTRL = 0x40, KMOD_RCTRL = 0x80,
	KMOD_LALT = 0x100, KMOD_RALT = 0x200, KMOD_NUM = 0x1000, KMOD_CAPS = 0x2000
} SDLMod;
#define KMOD_SHIFT (KMOD_LSHIFT | KMOD_RSHIFT)
#define KMOD_CTRL  (KMOD_LCTRL | KMOD_RCTRL)
#define KMOD_ALT   (KMOD_LALT | KMOD_RALT)
typedef struct { Uint8 scancode; SDLKey sym; SDLMod mod; Uint16 unicode; } SDL_keysym;

// ---- events ----
enum {
	SDL_NOEVENT = 0, SDL_ACTIVEEVENT, SDL_KEYDOWN, SDL_KEYUP, SDL_MOUSEMOTION,
	SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP, SDL_JOYAXISMOTION, SDL_JOYBALLMOTION,
	SDL_JOYHATMOTION, SDL_JOYBUTTONDOWN, SDL_JOYBUTTONUP, SDL_QUIT
};
#define SDL_RELEASED 0
#define SDL_PRESSED  1
#define SDL_BUTTON_LEFT   1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT  3
#define SDL_BUTTON(x) (1 << ((x) - 1))
#define SDL_BUTTON_LMASK SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK SDL_BUTTON(SDL_BUTTON_RIGHT)
typedef struct { Uint8 type, which, state; SDL_keysym keysym; } SDL_KeyboardEvent;
typedef struct { Uint8 type, which, state; Uint16 x, y; Sint16 xrel, yrel; } SDL_MouseMotionEvent;
typedef struct { Uint8 type, which, button, state; Uint16 x, y; } SDL_MouseButtonEvent;
typedef struct { Uint8 type, which, ball; Sint16 xrel, yrel; } SDL_JoyBallEvent;
typedef union {
	Uint8 type;
	SDL_KeyboardEvent key;
	SDL_MouseMotionEvent motion;
	SDL_MouseButtonEvent button;
	SDL_JoyBallEvent jball;
} SDL_Event;
typedef int (*SDL_EventFilter)(const SDL_Event *event);
int   SDL_PollEvent(SDL_Event *e);
void  SDL_SetEventFilter(SDL_EventFilter filter);
Uint8 SDL_GetMouseState(int *x, int *y);

// ---- joysticks: never any ----
typedef struct SDL_Joystick { int unused; } SDL_Joystick;
int  SDL_NumJoysticks(void);
SDL_Joystick *SDL_JoystickOpen(int i);
int  SDL_JoystickEventState(int state);
int  SDL_JoystickNumButtons(SDL_Joystick *j);
int  SDL_JoystickNumAxes(SDL_Joystick *j);
Sint16 SDL_JoystickGetAxis(SDL_Joystick *j, int axis);
Uint8 SDL_JoystickGetButton(SDL_Joystick *j, int button);
void SDL_JoystickClose(SDL_Joystick *j);
const char *SDL_JoystickName(int i);

#endif
