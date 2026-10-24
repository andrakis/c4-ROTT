# Rise of the Triad's data

`1rott13.zip` is Apogee's shareware package of Rise of the Triad v1.3,
3,668,139 bytes, unmodified, as icculus.org mirrors it. It holds the
first episode, "The HUNT Begins".

Its `VENDOR.DOC` allows the package to be passed on unchanged, with every
file in it, and marked as shareware. That is why c4 may fetch it and put
it on its disks, as it does Wolfenstein 3D's `1wolf14.zip`. The registered game and any modified
version may not be distributed.

`build-images.sh` doesn't change the package. It reads the game's files out
of it with `tools/rottdata.py` and puts them on the disks it builds:
- `HUNTBGIN.WAD`, `HUNTBGIN.RTL`, `HUNTBGIN.RTC`, `REMOTE1.RTS`;
- the four demos, `DEMO1_3.DMO` to `DEMO4_3.DMO`.
- `VENDOR.DOC`, which the game checks is there, and writes where it is not.

The package holds them in `ROTTSW13.SHR`, a DOS self-extracting archive
that Python's zipfile reads as it is.

## Getting it

`1rott13.zip` is not in this repository. In c4, `make rott-data` puts it here
with c4's `games/fetch.sh`:
- from `$ROTT_ZIP`, when that names your copy;
- or else from the first URL in `MIRRORS` that gives exactly this file.

Either way it is refused unless its SHA-256 is
`eea67a85845d52fa0ee7272e3ae24b79769ed1e8bdff7b1360904078bc51ad1e`.
Every URL in `MIRRORS` was checked to give exactly that file
(2026-10-25).
