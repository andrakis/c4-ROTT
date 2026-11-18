// acceltest.c -- c4's port: ROTT's accelerators (c4/accel.mjs) against the
// loops they stand for, each as rt_scale.c, rt_floor.c, rt_draw.c, rt_vid.c
// and watcom.c have them: the same pixels, on made-up textures, shades,
// steps and shifts, and the same 64-bit fixed-point results as __int64's.
// test-rott runs it on c4's libjs.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "c4accel.h"

int rott_accel = -1;
int rott_accel_on = 1;
int rott_accel_base(void) { return rott_accel = accel_find("rott"); }

unsigned seed = 12345;
int rnd(int n) { seed = seed * 1103515245 + 12345; return (int)((seed >> 8) % (unsigned)n); }

unsigned char *tex, *shade, *bufc, *bufa;
int fails, cases;
enum { TEXN = 131072 + 65536, SHN = 256, W = 320, H = 200, BUFN = W * H };

void fill(unsigned char *p, int n, int holes) {
    int i;
    for (i = 0; i < n; i++) { p[i] = rnd(256); if (holes && rnd(8) == 0) p[i] = 255; }
}
void check(char *what, int k) {
    cases++;
    if (memcmp(bufc, bufa, BUFN) != 0) {
        fails++;
        if (fails < 10) printf("accel: %s, case %d, differs\n", what, k);
    }
}
void clear() { fill(bufc, BUFN, 0); memcpy(bufa, bufc, BUFN); }

// R_DrawColumn (sgn: frac >> sh signed), R_DrawClippedColumn and
// R_DrawWallColumn (unsigned), the texture's middle in the middle of tex
void colc(int n, int frac, int step, int sh, int sgn, unsigned char *src, unsigned char *sh_, unsigned char *d) {
    while (n--) {
        *d = sh_[src[sgn ? (frac >> sh) : (int)(((unsigned)frac) >> sh)]];
        d += W; frac += step;
    }
}
void test_col() {
    int k, n, sh, sgn, frac, step, x, y;
    unsigned char *src;
    for (k = 0; k < 600; k++) {
        clear();
        sgn = k % 3 == 0; y = rnd(H); n = rnd(H - y + 1); x = rnd(W);
        if (k % 3 == 2) {    // R_DrawWallColumn: frac << 10, >> 26, a 64-texel post
            sh = 26; src = tex + rnd(4096);
            frac = rnd(1 << 30) * 4 + rnd(4); step = rnd(1 << 26);
        } else {             // a sprite's: 16.16, the source's middle the post's
            sh = 16; src = tex + 65536;
            frac = rnd(1 << 22) - (1 << 21); step = rnd(1 << 17);
            if (!sgn) frac = rnd(1 << 22);
        }
        colc(n, frac, step, sh, sgn, src, shade, bufc + x + y * W);
        if (n > 0) rott_col(n, frac, step, sh, sgn, src, shade, bufa + x + y * W, W);
        check(sgn ? "col (signed)" : sh == 26 ? "col (wall)" : "col", k);
    }
}

// R_TransColumn
void test_trans() {
    int k, n, x, y, i;
    unsigned char *d;
    for (k = 0; k < 300; k++) {
        clear();
        y = rnd(H); n = rnd(H - y + 1); x = rnd(W);
        for (d = bufc + x + y * W, i = n; i--; d += W) *d = shade[*d];
        if (n > 0) rott_trans(n, shade, bufa + x + y * W, W);
        check("trans", k);
    }
}

// DrawRow
void test_row() {
    int k, n, x, y, coord;
    unsigned frac, step;
    unsigned char *d;
    for (k = 0; k < 400; k++) {
        clear();
        y = rnd(H); x = rnd(W); n = rnd(W - x + 1);
        frac = (unsigned)rnd(1 << 30) * 4 + rnd(4); step = (unsigned)rnd(1 << 30) * 4 + rnd(4);
        if (k & 1) step = (unsigned)(rnd(1 << 12) - (1 << 11)) << 16 | rnd(65536);
        {
            unsigned f = frac; int c = n;
            d = bufc + x + y * W;
            while (c--) {
                coord = ((f >> (32-7)) | ((f >> (32-23)) << 7)) & 16383;
                *d++ = shade[tex[coord]];
                f += step;
            }
        }
        if (n > 0) rott_row(n, frac, step, tex, shade, bufa + x + y * W);
        check("row", k);
    }
}

// DrawRotRow at 320, 640 and 800 across, and DrawMaskedRotRow
void test_rotrow() {
    static int xlims[3] = { 256, 512, 640 }, ylims[3] = { 512, 922, 1024 }, shs[3] = { 9, 10, 10 };
    int k, n, y, c, masked, r, sh;
    unsigned xf, yf, xs, ys, eax, ex, ey;
    unsigned char *d;
    for (k = 0; k < 600; k++) {
        clear();
        masked = k & 1;
        r = masked ? 0 : (k >> 1) % 3; sh = shs[r];
        y = rnd(H); n = rnd(W + 1);
        xf = (unsigned)rnd(1 << 26) * 8 - (1 << 26); yf = (unsigned)rnd(1 << 26) * 16 - (1 << 27);
        xs = (unsigned)(rnd(1 << 18) - (1 << 17)); ys = (unsigned)(rnd(1 << 18) - (1 << 17));
        ex = xf; ey = yf; d = bufc + y * W; c = n;
        while (c--) {
            eax = ex >> 16;
            if (eax < (unsigned)xlims[r] && (ey >> 16) < (unsigned)ylims[r]) eax = (eax << sh) | ((ey << (16 - sh)) >> (32 - sh));
            else eax = 0;
            if (!masked || tex[eax] != 0xff) *d = tex[eax];
            d++;
            ex += xs; ey += ys;
        }
        if (n > 0) rott_rotrow(n, xf, yf, xs, ys, tex, bufa + y * W, masked, xlims[r], ylims[r], sh);
        check(masked ? "rotrow (masked)" : "rotrow", k);
    }
}

// DrawSkyPost at 320, and above it with its posts taller than 400 (a
// column 2 bytes wide here, so that 600 rows fit the buffer)
void test_sky() {
    int k, n, x, y, i, orgh, stride, height;
    unsigned char *d, *s, *src;
    for (k = 0; k < 300; k++) {
        clear();
        src = tex + 4096 + rnd(65536);
        if (k % 3 == 2) {
            stride = 2; y = 0; x = rnd(2); n = 401 + rnd(199); orgh = n;
            for (d = bufc + x, s = src, height = n; height--; d += stride) {
                if (orgh > 0 && height < orgh - 400) { s -= 2; *d = shade[*s]; }
                else *d = shade[*s];
                s++;
            }
        } else {
            stride = W; y = rnd(H); n = rnd(H - y + 1); x = rnd(W); orgh = 0;
            for (d = bufc + x + y * W, s = src, i = n; i--; d += W) *d = shade[*s++];
        }
        if (n > 0) rott_sky(n, src, shade, bufa + x + y * stride, stride, orgh);
        check("sky", k);
    }
}

// DrawTiledRegion (rt_vid.c), its four planes of a sw x sh pic
void tilec(unsigned char *start, int width, int height, int offx, int offy, unsigned char *source, int sw, int sh) {
    unsigned char *sourceoff, *origdest, *dest;
    int sourcex, sourcey, plane, hi, wi, startoffset = offy * sw, planesize = sw * sh;
    for (plane = 4; plane > 0; plane--) {
        origdest = start + (4 - plane);
        sourcey = offy; sourceoff = source + startoffset; hi = height;
        while (hi--) {
            dest = origdest; sourcex = offx; wi = width;
            while (wi--) {
                *dest = sourceoff[sourcex];
                dest += 4;
                if (++sourcex >= sw) sourcex = 0;
            }
            origdest += W; sourceoff += sw;
            if (++sourcey >= sh) { sourcey = 0; sourceoff = source; }
        }
        source += planesize;
    }
}
void test_tile() {
    int k, width, height, offx, offy, sw, sh, at;
    unsigned char *src;
    for (k = 0; k < 200; k++) {
        clear();
        sw = 1 + rnd(64); sh = 1 + rnd(64); offx = rnd(sw); offy = rnd(sh);
        width = 1 + rnd(60); height = 1 + rnd(100); at = rnd(W) + rnd(50) * W;
        src = tex + rnd(65536);
        tilec(bufc + at, width, height, offx, offy, src, sw, sh);
        rott_tile(bufa + at, width, height, offx, offy, src, sw, sh, W);
        check("tile", k);
    }
}

// rt_scale.c's four posts, by mode as RA_POST takes them, the globals they
// leave behind the test's own
int ylk[H], c_yl, c_yh, a_yl, a_yh;
unsigned char *c_source, *a_source;
void postc(unsigned char *src, unsigned char *buf, int top0, int inv, int vh, int tmid, int iscale,
           int center, unsigned char *sh_, unsigned char *see, int mode) {
    int offset, length, top, bot, n, frac;
    unsigned char *dest;
    offset = *src++;
    for (; offset != 255;) {
        length = *src++;
        top = top0 + inv * offset; bot = top + inv * length;
        c_yl = (top + ((mode & RA_POST_ROUND) ? 65535 : 65536)) >> 16;
        c_yh = (bot - 1) >> 16;
        if (c_yh >= vh) c_yh = vh - 1;
        if (c_yl < 0) c_yl = 0;
        if ((mode & RA_POST_TRANS) && *src == 254) {
            if (c_yl <= c_yh)
                for (dest = buf + ylk[c_yl], n = c_yh - c_yl + 1; n--; dest += W) *dest = see[*dest];
            src++;
            offset = *src++;
        } else {
            if (c_yl <= c_yh) {
                c_source = src - offset;
                dest = buf + ylk[c_yl];
                frac = tmid + (c_yl - center) * iscale;
                for (n = c_yh - c_yl + 1; n--; dest += W, frac += iscale)
                    *dest = sh_[c_source[(mode & RA_POST_CLIP) ? (int)(((unsigned)frac) >> 16) : (frac >> 16)]];
            }
            src += length;
            offset = *src++;
        }
    }
}
void test_post() {
    static int modes[4] = { 0, RA_POST_ROUND | RA_POST_CLIP, RA_POST_TRANS, RA_POST_CLIP | RA_POST_TRANS };
    int k, i, j, mode, off, len, top0, inv, tmid, iscale, center, x;
    unsigned char *p, *src, *see;
    for (i = 0; i < H; i++) ylk[i] = i * W;
    for (k = 0; k < 800; k++) {
        clear();
        mode = modes[k & 3];
        src = p = tex + 65536;          // the post: runs of offset, length, texels (or 254), then 255
        for (off = rnd(40); off < 250; off += len + rnd(40)) {
            len = 1 + rnd(254 - off);
            if (len > 60) len = 1 + rnd(60);
            *p++ = off; *p++ = len;
            if ((mode & RA_POST_TRANS) && rnd(3) == 0) *p++ = 254;
            else for (j = 0; j < len; j++) { *p = rnd(256); if (*p == 254 && j == 0) *p = 7; p++; }
        }
        *p++ = 255;
        inv = (1 << 12) + rnd(1 << 18); top0 = rnd(250 << 16) - (50 << 16);
        tmid = rnd(1 << 24); iscale = 1 + rnd(1 << 17); center = rnd(H);
        see = tex + rnd(65536); x = rnd(W);
        c_yl = a_yl = c_yh = a_yh = -7; c_source = a_source = 0;
        postc(src, bufc + x, top0, inv, H, tmid, iscale, center, shade, see, mode);
        rott_post(src, bufa + x, top0, inv, H, tmid, iscale, center, shade, see, ylk, W, mode, &a_yl, &a_yh, &a_source);
        cases++;
        if (memcmp(bufc, bufa, BUFN) != 0 || c_yl != a_yl || c_yh != a_yh || c_source != a_source) {
            fails++;
            if (fails < 10) printf("accel: post mode %d, case %d, differs\n", mode, k);
        }
    }
}

// watcom.c's helpers, as __int64 has them
int pick(int k) {
    switch (rnd(6)) {
    case 0: return rnd(65536) - 32768;
    case 1: return (int)((unsigned)rnd(1 << 30) * 4 + rnd(4));
    case 2: return k & 1 ? 0x7fffffff : (int)0x80000000;
    case 3: return rnd(1 << 24) - (1 << 23);
    case 4: return rnd(3) - 1;
    }
    return rnd(1 << 30) - (1 << 29);
}
void bad(char *what, int a, int b, int c, int r, int w) {
    fails++;
    if (fails < 10) printf("accel: %s %d %d %d: %d, not %d\n", what, a, b, c, r, w);
}
void test_math() {
    int k, a, b, c, s, r, w;
    for (k = 0; k < 1000; k++) {
        a = pick(k); b = pick(k + 1); c = pick(k + 2); s = 1 + rnd(31);
        r = rott_fixmul(a, b); w = (int)((((int64_t)a * (int64_t)b + (int64_t)0x8000) >> 16) & 0xffffffff);
        cases++; if (r != w) bad("fixmul", a, b, 0, r, w);
        r = rott_fixmulshift(a, b, s); w = (int)((((uint64_t)((int64_t)a * (int64_t)b)) >> s) & 0xffffffff);
        cases++; if (r != w) bad("fixmulshift", a, b, s, r, w);
        if (c == 0) c = 1 + rnd(1000);
        r = rott_fixdiv2(a, c); w = (int)((((int64_t)a * 65536) / (int64_t)c) & 0xffffffff);
        cases++; if (r != w) bad("fixdiv2", a, c, 0, r, w);
        r = rott_fixscale(a, b, c); w = (int)((((int64_t)a * (int64_t)b) / (int64_t)c) & 0xffffffff);
        cases++; if (r != w) bad("fixscale", a, b, c, r, w);
    }
}

int main() {
    if (ROTT_ACCEL() <= 0) { printf("accel: ROTT's are not here\n"); return 0; }
    tex = malloc(TEXN); shade = malloc(SHN); bufc = malloc(BUFN); bufa = malloc(BUFN);
    fill(tex, TEXN, 1); fill(shade, SHN, 0);
    test_col(); test_trans(); test_row(); test_rotrow(); test_sky(); test_tile(); test_post(); test_math();
    if (fails) printf("accel: %d of %d cases differ\n", fails, cases);
    else printf("accel: ROTT's, %d cases, each as its loop has it\n", cases);
    return fails != 0;
}
