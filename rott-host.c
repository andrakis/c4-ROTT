// rott-host.c -- Rise of the Triad on a host, built by gcc -m32: no display
// and no sound, only --timedemo's frames, the reference make test-rott
// holds the c4lc builds to

#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

void rott_home(void) {}
int rott_open_display(void) { return 0; }
void rott_close_display(void) {}
uint32_t plat_ms(void) {
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
void plat_sleep(uint32_t ms) { usleep(ms * 1000); }
// what gui.c asks the machine: here, nothing is fitted
int __c4_info(void) { return 0; }
