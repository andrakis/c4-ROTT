// rott-sum.c -- what every platform of this port shares: the clock, and
// --timedemo N, which plays the shareware's demo N (DEMO<N>_3.DMO) from
// the title loop at once and exits when it ends, printing
//
//   timedemo: demo 1: 2711 frames, checksum 1a2b3c4d
//
// a checksum of every frame shown, its palette with it. While timing the
// clock is virtual, as Wolf3D's, Doom's and Tyrian's are -- each reading a
// millisecond later, and a delay adding its wait -- so the frames are the
// same on any machine, and the gcc and c4lc builds must print the same
// line (make test-rott). There is no display and no sound then.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

uint32_t plat_ms(void);          // rott-c4ix.c, rott-bare.c, rott-host.c
void     plat_sleep(uint32_t ms);

static int timing;
static int tracing;              // --trace: each frame's checksum as it goes
static int demo;
static int frames;
static uint32_t sum;
static uint32_t now;

static void report(void) {
	printf("timedemo: demo %d: %d frames, checksum %08x\n", demo, frames, (unsigned) sum);
}

static int windowing;           // --window: in the Terminal's window (rott-c4ix.c)

// takes --timedemo N, --trace and --window off the command line; main's first act
void rott_home(void);           // rott-c4ix.c, rott-bare.c, rott-host.c

void rott_start(int *argc, char **argv) {
	int i, j;
	rott_home();
	for (i = 1; i < *argc; i++) {
		if (strcmp(argv[i], "--trace") != 0) continue;
		tracing = 1;
		for (j = i; j + 1 <= *argc; j++) argv[j] = argv[j + 1];
		*argc = *argc - 1;
		break;
	}
	for (i = 1; i < *argc; i++) {
		if (strcmp(argv[i], "--window") != 0) continue;
		windowing = 1;
		for (j = i; j + 1 <= *argc; j++) argv[j] = argv[j + 1];
		*argc = *argc - 1;
		break;
	}
	for (i = 1; i + 1 < *argc; i++) {
		if (strcmp(argv[i], "--timedemo") != 0) continue;
		demo = atoi(argv[i + 1]);
		if (demo < 1 || demo > 4) demo = 1;
		timing = 1;
		for (j = i; j + 2 <= *argc; j++) argv[j] = argv[j + 2];
		*argc = *argc - 2;
		atexit(report);
		return;
	}
}

int rott_headless(void) { return timing; }
int rott_windowed(void) { return windowing; }
int rott_timedemo(void) { return timing ? demo : 0; }

void rott_frame(uint8_t *pixels, int w, int h, int pitch, uint32_t *palette) {
	int i, y;
	uint32_t hh;
	uint8_t *p;
	if (!timing) return;
	hh = sum;
	for (i = 0; i < 256; i++) hh = hh * 31 + palette[i];
	for (y = 0; y < h; y++) {
		p = pixels + y * pitch;
		for (i = 0; i + 3 < w; i += 4)
			hh = hh * 31 + (uint32_t) (p[i] | (p[i + 1] << 8) | (p[i + 2] << 16) | ((uint32_t) p[i + 3] << 24));
	}
	sum = hh;
	frames++;
	if (tracing) printf("frame %d %08x\n", frames, (unsigned) hh);
}

uint32_t rott_ticks(void) {
	if (timing) return ++now;
	return plat_ms();
}

void rott_delay(uint32_t ms) {
	if (timing) { now += ms; return; }
	plat_sleep(ms);
}
