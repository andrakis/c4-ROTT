/* c4accel.h -- c4's port: ROTT's accelerators, c4/accel.mjs, from the
   game's side (c4's libjs/guest/accel.h, docs/accel.md). On a machine that
   has them, with Extended User Options' ACCELERATED on, the host
   runs these in the game's own JavaScript, one instruction each; elsewhere,
   and in gcc's build, the C runs as before. rott_accel is the first of
   ROTT's numbers (0 for none, -1 until asked: rt_scale.c); ROTT_ACCEL()
   asks once. The numbers are the module's operations in its order. */
#ifndef C4ACCEL_H
#define C4ACCEL_H

#include "accel.h"

enum {
	RA_COL, RA_TRANS, RA_ROW, RA_ROTROW, RA_SKY,
	RA_FIXMUL, RA_FIXMULSHIFT, RA_FIXDIV2, RA_FIXSCALE,
	RA_TILE, RA_POST
};
/* RA_POST's modes: a run's top rounded from SFRACUNIT - 1; its column
   R_DrawClippedColumn's; runs marked 254 translucent */
enum { RA_POST_ROUND = 1, RA_POST_CLIP = 2, RA_POST_TRANS = 4 };

extern int rott_accel;
int rott_accel_base(void);
int rott_accel_avail(void);      /* the machine has them */
void rott_accel_set(int on);     /* the menu's switch, and the config's */
extern int rott_accel_on;        /* ACCELERATED, in the config */
#define ROTT_ACCEL() (rott_accel >= 0 ? rott_accel : rott_accel_base())

#ifdef __c4_inttypes__
#define RA_OP(op) (rott_accel + (op))
/* a column: *dest = shade[src[frac >> sh]], the shift signed if sgn */
#define rott_col(n, frac, step, sh, sgn, src, shade, dest, stride) \
	__c4_opcode((n), (frac), (step), (sh), (sgn), (src), (shade), (dest), (stride), RA_OP(RA_COL))
#define rott_trans(n, shade, dest, stride) \
	__c4_opcode((n), (shade), (dest), (stride), RA_OP(RA_TRANS))
#define rott_row(n, frac, step, src, shade, dest) \
	__c4_opcode((n), (frac), (step), (src), (shade), (dest), RA_OP(RA_ROW))
#define rott_rotrow(n, xfrac, yfrac, xstep, ystep, src, dest, masked, xlim, ylim, sh) \
	__c4_opcode((n), (xfrac), (yfrac), (xstep), (ystep), (src), (dest), (masked), (xlim), (ylim), (sh), RA_OP(RA_ROTROW))
#define rott_sky(n, src, shade, dest, stride, orgh) \
	__c4_opcode((n), (src), (shade), (dest), (stride), (orgh), RA_OP(RA_SKY))
/* watcom.c's: FixedMul, FixedMulShift for 0 < s < 32, FixedDiv2 for b
   not 0, FixedScale for c not 0 */
#define rott_fixmul(a, b) __c4_opcode((a), (b), RA_OP(RA_FIXMUL))
#define rott_fixmulshift(a, b, s) __c4_opcode((a), (b), (s), RA_OP(RA_FIXMULSHIFT))
#define rott_fixdiv2(a, b) __c4_opcode((a), (b), RA_OP(RA_FIXDIV2))
#define rott_fixscale(a, b, c) __c4_opcode((a), (b), (c), RA_OP(RA_FIXSCALE))
/* rt_vid.c's DrawTiledRegion, the whole region */
#define rott_tile(start, width, height, offx, offy, source, sw, sh, stride) \
	__c4_opcode((start), (width), (height), (offx), (offy), (source), (sw), (sh), (stride), RA_OP(RA_TILE))
/* rt_scale.c's ScaleMaskedPost, ScaleClippedPost, ScaleTransparentPost and
   ScaleTransparentClippedPost, a post's every run, with the globals they
   read as arguments and those they leave by address */
#define rott_post(src, buf, top0, inv, vh, tmid, iscale, center, shade, see, ylk, stride, mode, pyl, pyh, psrc) \
	__c4_opcode((src), (buf), (top0), (inv), (vh), (tmid), (iscale), (center), (shade), (see), (ylk), (stride), \
		(mode), (pyl), (pyh), (psrc), RA_OP(RA_POST))
#else
#define rott_col(n, frac, step, sh, sgn, src, shade, dest, stride) 0
#define rott_trans(n, shade, dest, stride) 0
#define rott_row(n, frac, step, src, shade, dest) 0
#define rott_rotrow(n, xfrac, yfrac, xstep, ystep, src, dest, masked, xlim, ylim, sh) 0
#define rott_sky(n, src, shade, dest, stride, orgh) 0
#define rott_fixmul(a, b) 0
#define rott_fixmulshift(a, b, s) 0
#define rott_fixdiv2(a, b) 0
#define rott_fixscale(a, b, c) 0
#define rott_tile(start, width, height, offx, offy, source, sw, sh, stride) 0
#define rott_post(src, buf, top0, inv, vh, tmid, iscale, center, shade, see, ylk, stride, mode, pyl, pyh, psrc) 0
#endif

#endif
