# c4.mk -- this game's part of c4's Makefile. c4 includes it from
# games/rott/c4.mk, its submodule, and runs it from c4's root, with c4's
# tools and variables (games/README.md in c4). Without it, c4 builds
# everything else, and test-rott says it was skipped.

# Rise of the Triad (games/rott/README.md): icculus.org's port, its units
# with c4lc, the same objects for C4IX, the bare machine and C4DOS but for
# the platform's (rott-c4ix.c, rott-bare.c, in .rott/plat), linked by
# build-images.sh as Doom's are. rott-snd.c stands in for its SDL_mixer
# sound (fx_man.c, dukemusc.c), which is not built; nor are the units its
# own Makefile leaves out.
ROTT       := games/rott
ROTT_UNITS := cin_actr cin_efct cin_evnt cin_glob cin_main cin_util dosutil \
              engine isr modexlib rt_actor rt_battl rt_build rt_cfg rt_crc \
              rt_com rt_debug rt_dmand rt_door rt_draw rt_floor rt_game \
              rt_in rt_main rt_map rt_menu rt_msg rt_net rt_playr rt_rand \
              rt_scale rt_sound rt_spbal rt_sqrt rt_stat rt_state rt_str \
              rt_swift rt_ted rt_util rt_view rt_vid rt_err scriplib w_wad \
              watcom z_zone byteordr winrott c4sdl rott-sum rott-snd \
              midi al_midi gmtimbre ll_man usrhooks
ROTT_HDRS  := $(wildcard $(ROTT)/*.h $(ROTT)/c4/*.h) \
              $(wildcard src/c4ix/libc/include/*.h src/c4ix/libc/include/sys/*.h) \
              src/c4ix/include/display.h libjs/guest/gui.h libjs/guest/gui.c
ROTT_DEFS  := -D C4ROTT -D PLATFORM_UNIX=1 -D USE_SDL=1 -D SHAREWARE=1 -D SUPERROTT=0 -D SITELICENSE=0
ROTT_CC    := ./c4sp32 -c 400000 src/c4sp/lisp/c4lc.lisp -O $(IMG_G) -mfuse -fno-builtin -c -conforming \
              $(ROTT_DEFS) -I $(ROTT)/c4 -I src/c4ix/libc/include -I src/c4ix/include \
              -I libjs/guest -I $(ROTT)
rott-objs: $(ROTT_UNITS:%=.rott/%.c4o) .rott/plat/rott-c4ix.c4o .rott/plat/rott-bare.c4o
.rott/%.c4o: $(ROTT)/%.c $(ROTT_HDRS) $(C4LC_LISP) .img_g | c4sp32
	@mkdir -p .rott/plat
	$(ROTT_CC) $< $@ > /dev/null
.rott/plat/%.c4o: $(ROTT)/%.c $(ROTT_HDRS) $(C4LC_LISP) .img_g | c4sp32
	@mkdir -p .rott/plat
	$(ROTT_CC) $< $@ > /dev/null
PHONY += rott-objs

# ... and by gcc for the host, with no display and no sound (rott-host.c),
# the reference test-rott holds them to: 32-bit as they are, and with SSE's
# arithmetic, which is IEEE's as c4's is -- the x87's extra precision
# plays the demos differently at -O0 and -O2
ROTT_HOST_SRCS := $(ROTT_UNITS:%=$(ROTT)/%.c) $(ROTT)/rott-host.c
rott-host: $(ROTT_HOST_SRCS) $(ROTT_HDRS)
	gcc -m32 -O2 -std=gnu89 -w -fno-strict-aliasing -msse2 -mfpmath=sse \
	    -Wno-error=implicit-function-declaration -Wno-error=implicit-int \
	    -Wno-error=incompatible-pointer-types -Wno-error=int-conversion \
	    $(ROTT_DEFS) -I$(ROTT)/c4 -I$(ROTT) -Ilibjs/guest -o $@ $(ROTT_HOST_SRCS) -lm

# The data, which is not in the repository: data/1rott13.zip, the shareware
# package as it was distributed, checked by its SHA-256. c4's
# games/fetch.sh takes it from $ROTT_ZIP when that names a copy, or
# else from the mirrors in data/MIRRORS (data/README.md).
rott-data: $(ROTT)/data/1rott13.zip
$(ROTT)/data/1rott13.zip:
	@sh games/fetch.sh $@ eea67a85845d52fa0ee7272e3ae24b79769ed1e8bdff7b1360904078bc51ad1e ROTT_ZIP $$(cat $(ROTT)/data/MIRRORS 2>/dev/null)
PHONY += rott-data

# test-rott: the shareware's demo 4 (--timedemo, rott-sum.c: every frame
# shown and its palette, on a virtual clock), built by gcc and by c4lc --
# on the bare machine, under C4DOS and inside C4IX -- must show the same
# frames. The gcc build runs where it may write its config, beside links
# to the data.
ROTT_DEMO := 4
test-rott: rott-data rott-host c4bb-images
	@rm -rf .rott_t && mkdir -p .rott_t && \
	 for f in src/c4bb/images/games/*.WAD src/c4bb/images/games/*.RT? src/c4bb/images/games/*.DMO src/c4bb/images/games/VENDOR.DOC; do \
	   ln -s ../$$f .rott_t/; done && \
	 ( cd .rott_t && ../rott-host --timedemo $(ROTT_DEMO) NOSOUND 2>/dev/null | grep '^timedemo' > host.txt ) && \
	 cat .rott_t/host.txt && \
	 ( cd src/c4bb/images/games && { timeout 1500 node ../../../../libjs/cli.js -m 64 -d . rott.c4r --timedemo $(ROTT_DEMO) NOSOUND \
	     > ../../../../.rott_t/bare.out 2>/dev/null & \
	   printf 'rott --timedemo $(ROTT_DEMO) NOSOUND\nver\n' | timeout 1500 node ../../../../libjs/cli.js -m 64 -d . ../c4dos32.c4r \
	     > ../../../../.rott_t/dos.out 2>/dev/null; wait; } ) && \
	 printf 'rott --timedemo $(ROTT_DEMO) NOSOUND\nexit\n' | timeout 1500 node libjs/cli.js -m 64 -d src/c4bb/images/disk src/c4bb/images/c4ix32.c4r \
	   2>/dev/null > .rott_t/c4ix.out && \
	 for w in bare dos c4ix; do grep '^timedemo' .rott_t/$$w.out | cmp -s .rott_t/host.txt - \
	   || { echo "test-rott: $$w's frames differ:"; tail -5 .rott_t/$$w.out; exit 1; }; done && \
	 { grep -A1 '^timedemo' .rott_t/dos.out | grep -q '^A>C4DOS version' \
	   || { echo "test-rott: C4DOS did not come back to its prompt:"; tail -5 .rott_t/dos.out; exit 1; }; } && \
	 rm -rf .rott_t && echo "test-rott: OK"
PHONY += test-rott

GAME_OBJS  += rott-objs
GAME_TESTS += test-rott
