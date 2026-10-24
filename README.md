# src/rott — Rise of the Triad on C4IX, C4DOS and the bare machine

Rise of the Triad: Dark War (Apogee, 1995), as icculus.org's port has it.
It is compiled by c4lc and played on libjs's whole display, from the
keyboard, with the mouse turning the player. The shareware episode, "The
HUNT Begins", ships with it. Its music is the DOS game's own MIDI player
and AdLib driver on the sound card's OPL, and its effects play on the
card's sample channels (Sound, below).

[docs/ports-feasibility.md](../../docs/ports-feasibility.md) (§7) says why
icculus's port is the one to build on. The DOS release has Watcom's
`#pragma aux`, assembly drawers and Mode X, all of which icculus's port
already replaced with C. That section also says what c4lc needed, which
was M13 ([docs/c4lc-design.md](../../docs/c4lc-design.md) 14.12).

## Where it runs

| where | how | platform |
|---|---|---|
| C4IX, the whole display | `rott`, or Go > Rise of the Triad; the desktop gives way until it ends (`include/display.h`) | `c4sdl.c`, `rott-c4ix.c` |
| C4IX, a window | `rott --window` from a Terminal, which becomes the game's window (`include/window.h`), as `wolf3d --window`, `doom -window` and `tyrian --window` play; also when a program that does not give way has the display | `c4sdl.c`, `rott-c4ix.c` |
| C4DOS | `ROTT` at the `A>` prompt; the page's `?system=c4dos` | `c4sdl.c`, `rott-bare.c` |
| the bare machine | `rott.c4r` as the machine's image; the page's `?system=rott` | `c4sdl.c`, `rott-bare.c` |

On C4IX the game is in `/usr/games/rott` with the episode's files. It
starts there, its config and saved games go there, and it asks for a nice
of -5 (docs/c4ix-design.md 4.4). On the bare machine and under C4DOS the
files are on the games' disk beside it.

The mouse is locked to the display from a click on it, and Esc lets it go
(docs/libjs-design.md, `GUI_PTR_LOCK`). Its motion turns the player as
SDL's relative motion did. In a window the game takes only the keys, as
the other windowed games do.

## Where it comes from

- **The game:** [icculus.org's port](https://icculus.org/rott/) of the
  1.4 source Apogee released in 2002, version 1.1.2, under the GNU GPL v2
  (`COPYING`). `README.icculus` is its own README. Its game units and
  headers are here, and of its `audiolib` (Apogee's sound library, which
  the DOS release linked and icculus's port does not build) the five
  units the music needs. Its Mac and Visual Studio directories are not.
- **The data:** Apogee's shareware package, `data/1rott13.zip`,
  unmodified (`data/README.md`). `tools/rottdata.py` reads the game's
  files out of it for the disks.

## What is here

| file | what |
|---|---|
| icculus's units and headers | the game, with the changes below |
| `c4/SDL.h`, `c4sdl.c` | the slice of SDL 1.2 the game calls: its 8-bit screen and palette sent as `GUI_BLIT8`, which the host scales to 640x480; the keyboard as SDL's keysyms; the mouse |
| `rott-snd.c` | the game's FX_ and MUSIC_ interfaces on libjs's sound card, in place of `fx_man.c` and `dukemusc.c` (SDL_mixer), which are not built; and the poll that runs the music |
| `midi.c`, `al_midi.c`, `gmtimbre.c`, `ll_man.c`, `usrhooks.c` and their headers | five units of Apogee's `audiolib`: the MIDI player, the AdLib driver, its General MIDI timbre bank, and what they use |
| `rott-sum.c` | what every platform shares: `--timedemo N` and `--window` off the command line, each frame shown checksummed with its palette, on a virtual clock |
| `rott-c4ix.c`, `rott-bare.c` | whether the display has to be asked for, where the game starts, and on C4IX the window (`--window`) |
| `rott-host.c` | the platform on a host, built with gcc: no display and no sound, the reference for `make test-rott` |
| `c4/` | `SDL.h`, a `signal.h`, and the `stdint.h`, `stdbool.h` and `inttypes.h` C4IX's libc lacks; and for `audiolib`, a `conio.h` whose port writes go to the card's OPL, and the DOS headers (`dos.h`, `dpmi.h`, `interrup.h`, `blaster.h`, `user.h`) as next to nothing |
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
  timedemo: demo 1: 1613 frames, checksum 880c2201
  timedemo: demo 2: 880 frames, checksum b4a61d41
  timedemo: demo 3: 1081 frames, checksum 6ae88033
  timedemo: demo 4: 609 frames, checksum 3b083b33
  ```

  On the bare machine, under libjs's `cli.js`, the four take 49, 24, 31
  and 19 seconds.

## How fast

In the page, on the machine this was measured on, the first level played
at about 14 frames a second on the bare machine (`?system=rott`, paced at
400 MHz, the host managing about 70M instructions a second).

## Sound

`rott-snd.c` puts the game's two sound interfaces, Apogee's `FX_` and
`MUSIC_`, on libjs's sound card (`libjs/guest/snd.h`). With no card, the
game is told so and plays silent, as on a DOS machine without one.

- **Music** is the DOS game's own. `audiolib`'s MIDI player (`midi.c`)
  plays the WAD's songs through its AdLib driver (`al_midi.c`) with the
  General MIDI timbre bank (`gmtimbre.c`), as the game did on an AdLib.
  - The card's OPL stands where the AdLib was: the driver's writes to
    ports 0x388 and 0x389 become the card's register writes
    (`rott_outp`).
  - Apogee's extended MIDI picks each song's AdLib tracks.
  - On DOS the player ran from the timer interrupt, through `audiolib`'s
    task manager. Here the task manager is a table in `rott-snd.c`, and
    `rott_snd_poll()` runs it. `c4sdl.c` calls the poll whenever the
    game reads the clock, which its waits do over and over, whenever it
    reads its input, which a menu waiting for a key does and nothing
    else, and with every frame.
  - The poll runs the player a tick at a time, with `snd_wait` between
    ticks, so each write is stamped with the stream's time, until the
    stream is a quarter of a second ahead. Music then plays evenly however
    unevenly the game runs. When a level is loading, the card brings the
    stream up to now, and the music waits rather than rushing.
  - Fades are `music.c`'s, on the same task manager.
- **Effects** are the WAD's VOC and WAV lumps, played as they are on the
  card's eight channels of 8-bit samples.
  - A sound's pitch offset, in cents, moves its rate, as `audiolib`'s
    `PITCH_GetScale` did.
  - A positioned sound is panned by angle and distance with
    `multivoc.c`'s table: 32 angles, and the far ear falls to nothing
    over a quarter turn.
  - A finished sound is let go, and the game is told by its callback, at
    the next poll.
  - The priorities decide who gets a channel when all eight are playing.

The `C4ROTT` changes in `audiolib` are four:
- `AL_Init` asks no Sound Blaster which OPL it has;
- `MIDI_Reset` does not wait a twenty-fourth of a second on DOS's clock;
- `_MIDI_InitEMIDI` picks the AdLib's tracks;
- `_midi.h` declares the service routine static, as `midi.c` defines it,
  which gcc insists on.

`--timedemo` with `NOSOUND` starts neither interface, and gcc's build
never touches the card. With music on, a demo is timed differently.
The timedemo clock moves a millisecond each time it is read, and
between songs the game reads it in a loop until the fade is done
(`MU_FadeToSong`). Demo 4 then shows 582 frames, not 609.
