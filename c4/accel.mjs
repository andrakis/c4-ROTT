// accel.mjs -- Rise of the Triad's accelerators for c4's libjs (its
// libjs/accel.js, docs/accel.md): the renderer's innermost loops and the
// 64-bit fixed-point helpers, run by the host when the machine has them and
// Extended User Options' ACCELERATED is on. c4/c4accel.h is the
// game's side, with each operation's number (RA_*); rt_scale.c, rt_floor.c,
// rt_draw.c, rt_vid.c and watcom.h call them, and keep their own C for
// every other machine. Each does exactly what that C does: the same pixels
// in the same order, 32-bit arithmetic wrapping, shifts as c4's SHL, SHR
// and LSR take them, the 64-bit ones as __int64 has them.
export default function rott({ reader, writer, reader32, writer32, probe, args }) {
  // RA_COL (n, frac, step, sh, sgn, src, shade, dest, stride): a column,
  // R_DrawColumn, R_DrawClippedColumn, R_DrawWallColumn: *dest =
  // shade[src[frac >> sh]]; dest += stride; frac += step. sgn: frac's shift
  // is signed (R_DrawColumn's int), else unsigned.
  function col(m) {
    let [n, frac, step, sh, sgn, src, shade, dest, stride] = args(m, 9);
    sh &= 31;
    const rs = reader(m), rp = reader(m), wr = writer(m);
    for (; n > 0; n--, dest = (dest + stride) | 0, frac = (frac + step) | 0)
      wr(dest, rp((shade + rs((src + (sgn ? frac >> sh : frac >>> sh)) | 0)) | 0));
    return 0;
  }

  // RA_TRANS (n, shade, dest, stride): R_TransColumn, a translucent post's
  // pixels through its shade, *dest = shade[*dest]. A blend run twice
  // blends twice: every pixel is worked out first, and the pages it writes
  // made sure of (probe), before the first is written.
  function trans(m) {
    const [n, shade, dest, stride] = args(m, 4);
    if (n <= 0) return 0;
    const rd = reader(m), rp = reader(m), out = new Uint8Array(n);
    let d = dest;
    for (let i = 0; i < n; i++, d = (d + stride) | 0) out[i] = rp((shade + rd(d)) | 0);
    let page = -1;
    d = dest;
    for (let i = 0; i < n; i++, d = (d + stride) | 0)
      if ((d >>> 12) !== page) { probe(m, d, 1, true); page = d >>> 12; }
    const wr = writer(m);
    d = dest;
    for (let i = 0; i < n; i++, d = (d + stride) | 0) wr(d, out[i]);
    return 0;
  }

  // RA_ROW (n, frac, step, src, shade, dest): a floor's or ceiling's row,
  // DrawRow, a 128x128 tile: *dest++ = shade[src[((frac >> 25) | ((frac >>
  // 9) << 7)) & 16383]]; frac += step, unsigned
  function row(m) {
    let [n, frac, step, src, shade, dest] = args(m, 6);
    const rs = reader(m), rp = reader(m), wr = writer(m);
    for (; n > 0; n--, dest = (dest + 1) | 0, frac = (frac + step) | 0)
      wr(dest, rp((shade + rs((src + (((frac >>> 25) | ((frac >>> 9) << 7)) & 16383)) | 0)) | 0));
    return 0;
  }

  // RA_ROTROW (n, xfrac, yfrac, xstep, ystep, src, dest, masked, xlim,
  // ylim, sh): the turned screen's row, DrawRotRow and DrawMaskedRotRow,
  // out of the image: inside it (x below xlim, y below ylim), (x << sh) |
  // the top sh bits of y's fraction, else texel 0; the masked one leaves
  // 255 out. At 320 across 256, 512, 9; at 640 512, 922, 10 (y < 512 *
  // 1.8); at 800 640, 1024, 10.
  function rotrow(m) {
    let [n, xf, yf, xs, ys, src, dest, masked, xlim, ylim, sh] = args(m, 11);
    sh &= 31;
    const yl = (16 - sh) & 31, yr = (32 - sh) & 31;
    const rs = reader(m), wr = writer(m);
    for (; n > 0; n--, dest = (dest + 1) | 0, xf = (xf + xs) | 0, yf = (yf + ys) | 0) {
      const x = xf >>> 16;
      const e = x < xlim && (yf >>> 16) < ylim ? ((x << sh) | ((yf << yl) >>> yr)) : 0;
      const c = rs((src + e) | 0);
      if (!masked || c !== 255) wr(dest, c);
    }
    return 0;
  }

  // RA_SKY (n, src, shade, dest, stride, orgh): DrawSkyPost, *dest =
  // shade[*src++]; dest += stride. Above 320 across a post taller than the
  // sky's 400 (orgh, else 0) goes back over the sky for its last orgh -
  // 400 pixels, two texels back each (the port's "bna fix")
  function sky(m) {
    let [n, src, shade, dest, stride, orgh] = args(m, 6);
    const rs = reader(m), rp = reader(m), wr = writer(m);
    while (n-- > 0) {
      if (orgh > 0 && n < orgh - 400) src = (src - 2) | 0;
      wr(dest, rp((shade + rs(src)) | 0));
      dest = (dest + stride) | 0;
      src = (src + 1) | 0;
    }
    return 0;
  }

  // RA_TILE (start, width, height, offx, offy, source, sw, sh, stride): a
  // region tiled with a pic's four planes, DrawTiledRegion as icculus has
  // it: plane by plane, each pixel of a row four bytes on from the last
  function tile(m) {
    let [start, width, height, offx, offy, source, sw, sh, stride] = args(m, 9);
    const rs = reader(m), wr = writer(m);
    const startoffset = Math.imul(offy, sw), planesize = Math.imul(sw, sh);
    for (let plane = 4; plane > 0; plane--) {
      let origdest = (start + (4 - plane)) | 0;
      let sy = offy, soff = (source + startoffset) | 0;
      for (let h = height; h > 0; h--) {
        let dest = origdest, sx = offx;
        for (let w = width; w > 0; w--) {
          wr(dest, rs((soff + sx) | 0));
          dest = (dest + 4) | 0;
          if (++sx >= sw) sx = 0;
        }
        origdest = (origdest + stride) | 0;
        soff = (soff + sw) | 0;
        if (++sy >= sh) { sy = 0; soff = source; }
      }
      source = (source + planesize) | 0;
    }
    return 0;
  }

  // RA_POST (src, buf, top0, inv, vh, tmid, iscale, center, shade, see, ylk,
  // stride, mode, pyl, pyh, psrc): a sprite's post, every run of it, as
  // rt_scale.c's ScaleMaskedPost (mode 0), ScaleClippedPost (3),
  // ScaleTransparentPost (4) and ScaleTransparentClippedPost (6) have it:
  // mode 1, a run's top rounded up from SFRACUNIT - 1; 2, its column
  // R_DrawClippedColumn's (frac unsigned, center centeryclipped), else
  // R_DrawColumn's; 4, a run marked 254 is translucent, R_TransColumn
  // through see. dc_yl, dc_yh and dc_source (pyl, pyh, psrc) are left as
  // the C leaves them. A translucent post reads the screen: every pixel is
  // worked out first and the pages written made sure of (probe) before
  // anything is written, so that running it again does not blend twice.
  function post(m) {
    let [src, buf, top0, inv, vh, tmid, iscale, center, shade, see, ylk, stride, mode, pyl, pyh, psrc] = args(m, 16);
    const round = mode & 1 ? 65535 : 65536, clip = mode & 2, transp = mode & 4;
    const rs = reader(m), rx = reader(m), rp = reader(m), ry = reader32(m);
    const wr = transp ? null : writer(m);
    const wa = [], wv = [], pend = transp ? new Map() : null;
    const put = (a, v) => { if (transp) { pend.set(a, v); wa.push(a); wv.push(v); } else wr(a, v); };
    let yl = 0, yh = 0, set = false, dsrc = 0, dset = false;
    let off = rs(src); src = (src + 1) | 0;
    while (off !== 255) {
      const len = rs(src); src = (src + 1) | 0;
      const top = (top0 + Math.imul(inv, off)) | 0;
      const bot = (top + Math.imul(inv, len)) | 0;
      yl = ((top + round) | 0) >> 16;
      yh = ((bot - 1) | 0) >> 16;
      if (yh >= vh) yh = (vh - 1) | 0;
      if (yl < 0) yl = 0;
      set = true;
      if (transp && rs(src) === 254) {
        if (yl <= yh) {
          let d = (buf + ry((ylk + 4 * yl) | 0)) | 0;
          for (let n = yh - yl + 1; n > 0; n--, d = (d + stride) | 0) {
            const c = pend.has(d) ? pend.get(d) : rx(d);
            put(d, rp((see + c) | 0));
          }
        }
        src = (src + 1) | 0;
        off = rs(src); src = (src + 1) | 0;
      } else {
        if (yl <= yh) {
          dsrc = (src - off) | 0; dset = true;
          let d = (buf + ry((ylk + 4 * yl) | 0)) | 0;
          let frac = (tmid + Math.imul((yl - center) | 0, iscale)) | 0;
          for (let n = yh - yl + 1; n > 0; n--, d = (d + stride) | 0, frac = (frac + iscale) | 0)
            put(d, rp((shade + rs((dsrc + (clip ? frac >>> 16 : frac >> 16)) | 0)) | 0));
        }
        src = (src + len) | 0;
        off = rs(src); src = (src + 1) | 0;
      }
    }
    if (transp) {
      let page = -1;
      for (const a of wa) if ((a >>> 12) !== page) { probe(m, a, 1, true); page = a >>> 12; }
      if (set) { probe(m, pyl, 4, true); probe(m, pyh, 4, true); }
      if (dset) probe(m, psrc, 4, true);
      const w = writer(m);
      for (let i = 0; i < wa.length; i++) w(wa[i], wv[i]);
    }
    const w32 = writer32(m);
    if (set) { w32(pyl, yl); w32(pyh, yh); }
    if (dset) w32(psrc, dsrc);
    return 0;
  }

  // (int64_t)a * b as its two words, hi signed and lo unsigned: the 16-bit
  // digits, the high ones signed, as watcom.c's C4_MUL64
  const mul64 = (a, b) => {
    const ah = a >> 16, al = a & 0xffff, bh = b >> 16, bl = b & 0xffff;
    const m1 = ah * bl, m2 = al * bh;
    let lo = (al * bl) >>> 0;
    let hi = (ah * bh + (m1 >> 16) + (m2 >> 16)) | 0;
    const t = (lo + ((m1 << 16) >>> 0)) >>> 0;
    if (t < lo) hi = (hi + 1) | 0;
    lo = (t + ((m2 << 16) >>> 0)) >>> 0;
    if (lo < t) hi = (hi + 1) | 0;
    return [hi, lo];
  };

  // RA_FIXMUL (a, b): FixedMul, (int32_t)(((int64_t)a * b + 0x8000) >> 16)
  function fixmul(m) {
    const [a, b] = args(m, 2);
    let [hi, lo] = mul64(a, b);
    const t = (lo + 0x8000) >>> 0;
    if (t < lo) hi = (hi + 1) | 0;
    return (t >>> 16) | (hi << 16);
  }

  // RA_FIXMULSHIFT (a, b, s), 0 < s < 32: FixedMulShift,
  // (int32_t)((uint64_t)((int64_t)a * b) >> s)
  function fixmulshift(m) {
    const [a, b, s] = args(m, 3);
    const [hi, lo] = mul64(a, b);
    return (lo >>> s) | (hi << (32 - s));
  }

  // RA_FIXDIV2 (a, b), b not 0: FixedDiv2, (int32_t)((int64_t)a * 65536 / b);
  // RA_FIXSCALE (a, b, c), c not 0: FixedScale, (int32_t)((int64_t)a * b / c).
  // A quotient toward zero, as __int64's, of products within 2^62.
  const low32 = x => Number(BigInt.asIntN(32, x)) | 0;
  function fixdiv2(m) {
    const [a, b] = args(m, 2);
    return low32((BigInt(a) << 16n) / BigInt(b));
  }
  function fixscale(m) {
    const [a, b, c] = args(m, 3);
    return low32(BigInt(a) * BigInt(b) / BigInt(c));
  }

  return [col, trans, row, rotrow, sky, fixmul, fixmulshift, fixdiv2, fixscale, tile, post];
}
