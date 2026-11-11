# c4-images.sh c4ix|bare -- Rise of the Triad's part of c4's
# build-images.sh, which sources it from c4's root (games/README.md in c4)
GAMEDIR=games/rott
# what rottreg, the registered game, takes from .rott/: the units that come
# out the same with SHAREWARE 0 (c4.mk, ROTT_SHARED)
rott_shared() { for u in c4sdl rott-sum midi al_midi gmtimbre ll_man usrhooks rt_err; do echo .rott/$u.c4o; done; }
case "$1" in
c4ix)
    # linked against C4IX's libc, with the shareware episode's files beside
    # it, out of Apogee's package as it ships (data/, tools/rottdata.py;
    # c4ix.vfs.txt: /usr/games/rott)
    if ls .rott/*.c4o > /dev/null 2>&1 && [ -f $PB/lib/libc.c4s ]; then
        say "linking Rise of the Triad for C4IX, and unpacking its shareware episode"
        ./c4rlink32 .rott/*.c4o .rott/plat/rott-c4ix.c4o -l $PB/lib/libc.c4s -o $DISK/c4ix-rott.c4r > /dev/null
        if [ -f $GAMEDIR/data/1rott13.zip ]; then python3 -I $GAMEDIR/tools/rottdata.py $GAMEDIR/data/1rott13.zip $DISK > /dev/null; fi
    fi
    # the registered game, whose files are the person's own: none here
    if ls .rottreg/*.c4o > /dev/null 2>&1 && [ -f $PB/lib/libc.c4s ]; then
        ./c4rlink32 .rottreg/*.c4o $(rott_shared) .rott/plat/rott-c4ix.c4o -l $PB/lib/libc.c4s -o $DISK/c4ix-rottreg.c4r > /dev/null
    fi ;;
bare)
    if ls .rott/*.c4o > /dev/null 2>&1; then
        item rott
        ./c4rlink32 .rott/*.c4o .rott/plat/rott-bare.c4o .libc-bare/libc-*.c4o -o $GAMESDISK/rott.c4r > /dev/null
        if [ -f $GAMEDIR/data/1rott13.zip ]; then python3 -I $GAMEDIR/tools/rottdata.py $GAMEDIR/data/1rott13.zip $GAMESDISK > /dev/null; fi
        GAMES_ON_DISK="$GAMES_ON_DISK ROTT"
    fi
    if ls .rottreg/*.c4o > /dev/null 2>&1; then
        item rottreg
        ./c4rlink32 .rottreg/*.c4o $(rott_shared) .rott/plat/rott-bare.c4o .libc-bare/libc-*.c4o -o $GAMESDISK/rottreg.c4r > /dev/null
    fi ;;
esac
