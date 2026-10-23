# src/rott — Rise of the Triad on C4IX, C4DOS and the bare machine

Rise of the Triad: Dark War (Apogee, 1995), as icculus.org's port has it.
It is compiled by c4lc and played on libjs's whole display, from the
keyboard, with the mouse turning the player. The shareware episode, "The
HUNT Begins", ships with it. There is no sound yet (below).

[docs/ports-feasibility.md](../../docs/ports-feasibility.md) (§7) says why
icculus's port is the one to build on. The DOS release has Watcom's
`#pragma aux`, assembly drawers and Mode X, all of which icculus's port
already replaced with C. That section also says what c4lc needed, which
was M13 ([docs/c4lc-design.md](../../docs/c4lc-design.md) 14.12).

## Where it runs

| where | how | platform |
|---|---|---|
| C4IX, the whole display | `rott`, or Go > Rise of the Triad; the desktop gives way until it ends (`include/display.h`) | `c4sdl.c`, `rott-c4ix.c` |
| C4DOS | `ROTT` at the `A>` prompt; the page's `?system=c4dos` | `c4sdl.c`, `rott-bare.c` |
| the bare machine | `rott.c4r` as the machine's image; the page's `?system=rott` | `c4sdl.c`, `rott-bare.c` |

On C4IX the game is in `/usr/games/rott` with the episode's files. It
starts there, its config and saved games go there, and it asks for a nice
of -5 (docs/c4ix-design.md 4.4). On the bare machine and under C4DOS the
files are on the games' disk beside it.

The mouse is locked to the display from a click on it, and Esc lets it go
(docs/libjs-design.md, `GUI_PTR_LOCK`). Its motion turns the player as
SDL's relative motion did.

## Where it comes from

- **The game:** [icculus.org's port](https://icculus.org/rott/) of the
  1.4 source Apogee released in 2002, version 1.1.2, under the GNU GPL v2
  (`COPYING`). `README.icculus` is its own README. Its game units and
  headers are here; its `audiolib`, Mac and Visual Studio directories are
  not.
- **The data:** Apogee's shareware package, `data/1rott13.zip`,
  unmodified (`data/README.md`). `tools/rottdata.py` reads the game's
  files out of it for the disks.

## What is here

| file | what |
|---|---|
| icculus's units and headers | the game, with the changes below |
| `c4/SDL.h`, `c4sdl.c` | the slice of SDL 1.2 the game calls: its 8-bit screen and palette sent as `GUI_BLIT8`, which the host scales to 640x480; the keyboard as SDL's keysyms; the mouse |
| `rott-snd.c` | the game's FX_ and MUSIC_ interfaces, in place of `fx_man.c` and `dukemusc.c` (SDL_mixer), which are not built: for now there is no card |
| `rott-sum.c` | what every platform shares: `--timedemo N`, each frame shown checksummed with its palette, on a virtual clock |
| `rott-c4ix.c`, `rott-bare.c` | whether the display has to be asked for, and where the game starts |
| `rott-host.c` | the platform on a host, built with gcc: no display and no sound, the reference for `make test-rott` |
| `c4/` | `SDL.h`, a `signal.h`, and the `stdint.h`, `stdbool.h` and `inttypes.h` C4IX's libc lacks |
| `tools/rottdata.py`, `data/` | the package and what reads it |

## The changes to icculus's port

Each is under `#ifdef C4ROTT`. Everything else is the port's own code.
- **The platform:** `rt_def.h` accepts the c4 build. `dosutil.c` keeps the
  game's files where it runs, instead of in `~/.rott`, and has no glibc
  backtrace.
- **320x200**, the original's size, is the default (`winrott.c`). It is a
  fifth of the work of the port's 640x480, and a config can still ask for
  more.
- **`--timedemo N`** (`rt_main.c`): the title loop plays demo N at once and
  quits when it ends, and the start-up screens are skipped. The random
  tables' seed is the wall clock's second, so while timing it is 0
  (`rt_rand.c`).
- **No `alloca`** in c4lc: the WAD directory is `malloc`ed (`w_wad.c`).
- **The zone** is zeroed when it is made (`z_zone.c`). The loading bar no
  longer walks every block in it for every lump it loads, which made
  loading a level quadratic, minutes on c4 (`rt_ted.c`).
- **The credits' background** is copied as the screen's rows, not Mode
  X's 400 (`rt_draw.c`). At 320x200 icculus's copy was twice the screen,
  and it ran 64 KB over the next heap block: libjs's display ring, so the
  display stopped. The port only ever ran at 640x480 and up, where it
  fits.
- **A cinematic's step** is at most 8 tics a frame (`cin_main.c`). The
  game measures it once from four frames. Paced, with the machine slower
  than its clock, libjs moves the clock on, and the measurement came out
  in the hundreds: each frame then took minutes, and a key went unseen.
- **A disk that takes no writes**, as the bare machine's, C4DOS's and the
  page's are: the config and saved games are not kept, and the game goes
  on (`rt_util.c`, `rt_cfg.c`). `VENDOR.DOC` is not written back out of
  the WAD (`rt_cfg.c`); the package's copy is on the disk.

## C4 bugs it found

- **libc's varargs.** A variadic call's arguments went on a 128-word stack
  of the library's own, and only the callee's `va_end` took them off.
  ROTT's `SoftError` has its body compiled out, so every call left words
  behind, and the stack grew past its end into the zone's header. Then an
  allocation looped for ever. The stack is now 1024 words, and a list that
  would not fit starts again at the bottom (`libc/bareu.c`,
  `lib/libc4ix.c`).
- **`(*pf)(x)` with `pf` a pointer to a function pointer** called `pf`
  itself. The parser dropped the `*` whatever the type was; the generator
  now drops it only for a function pointer (M13).

## Building and testing

- `make rott-objs` compiles each unit with c4lc into `.rott/`, and the
  two platform choices into `.rott/plat/`. `build-images.sh` links them
  two ways:
  - against C4IX's libc, as `/usr/games/rott/rott.c4r`;
  - whole with the bare libc, as `images/games/rott.c4r`.

  It unpacks the episode beside both.
- `make rott-host` is the gcc build, 32-bit as the c4lc build is, and with
  SSE's arithmetic. That is IEEE's, as c4's is; the x87's extra precision
  played the demos differently at `-O0` and `-O2`.
- `make test-rott` plays demo 4 with `--timedemo 4 NOSOUND` four ways: the
  gcc build, then c4lc's on the bare machine, under C4DOS and inside C4IX.
  All four must print the same frames:

  ```
  timedemo: demo 1: 1613 frames, checksum ef4d0701
  timedemo: demo 2: 880 frames, checksum b3aac541
  timedemo: demo 3: 1081 frames, checksum 6ae88033
  timedemo: demo 4: 609 frames, checksum 3c4e9b33
  ```

  On the bare machine, under libjs's `cli.js`, the four take 49, 24, 31
  and 19 seconds.

## How fast

In the page, on the machine this was measured on, the first level played
at about 14 frames a second on the bare machine (`?system=rott`, paced at
400 MHz, the host managing about 70M instructions a second).

## Sound, next

The DOS game played its music through Apogee's own AdLib MIDI driver,
`audiolib`'s `al_midi.c` and `midi.c` with the GM timbre bank. libjs's
sound card is an OPL, so that driver can drive it as it drove the card.
The effects go on the card's eight sample channels, as Doom's and
Tyrian's do. Both are to come, in `rott-snd.c`.
