#!/usr/bin/env python3
# rottdata.py ZIP DEST -- the files Rise of the Triad reads, out of Apogee's
# shareware package (1rott13.zip, as distributed) into DEST, flat, under
# the names they have there. The package holds a self-extracting archive,
# ROTTSW13.SHR: a DOS program with a zip behind it, which Python's zipfile
# reads as it is. The DOS programs and documents stay in it.
import io, os, sys, zipfile

KEEP = ('.WAD', '.RTL', '.RTC', '.RTS', '.DMO')

def main():
    if len(sys.argv) != 3:
        sys.exit('usage: rottdata.py ZIP DEST')
    outer = zipfile.ZipFile(sys.argv[1])
    inner = zipfile.ZipFile(io.BytesIO(outer.read('ROTTSW13.SHR')))
    dest = sys.argv[2]
    os.makedirs(dest, exist_ok=True)
    n = 0
    for i in inner.infolist():
        name = os.path.basename(i.filename)
        # VENDOR.DOC too: the game checks it is there, and writes it out of
        # the WAD where it is not -- which the bare machine cannot
        if not name.upper().endswith(KEEP) and name.upper() != "VENDOR.DOC":
            continue
        with open(os.path.join(dest, name), 'wb') as f:
            f.write(inner.read(i))
        n += 1
    if not os.path.exists(os.path.join(dest, 'HUNTBGIN.WAD')):
        sys.exit('rottdata.py: %s is not the shareware package (no HUNTBGIN.WAD)' % sys.argv[1])
    print('rottdata.py: %d files in %s' % (n, dest))

main()
