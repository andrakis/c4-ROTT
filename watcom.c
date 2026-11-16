#include "rt_def.h"

#include "watcom.h"

#if defined(C4ROTT) && defined(__c4_inttypes__)
/* c4: watcom.h's macros take these names where the accelerators do not */
#undef FixedMul
#undef FixedMulShift
#undef FixedDiv2
#undef FixedScale
#define FixedMul c4_FixedMul
#define FixedMulShift c4_FixedMulShift
#define FixedDiv2 c4_FixedDiv2
#define FixedScale c4_FixedScale
#endif

/* 
  C versions of watcom.h assembly.
  Uses the '__int64' type (see rt_def.h).
 */

#ifdef C4ROTT
/* c4: the 64-bit product in 16-bit digits, the high ones signed, from c4's
   32-bit operations -- __int64 is a call for every operation, four of them
   for a FixedMul. The cross products stay within 2^31 and the sum needs
   two carries. The same values as below, for every a and b. */
#define C4_MUL64(a, b, hi, lo) do { int ah_, bh_, m1_, m2_; unsigned al_, bl_, t_; \
	ah_ = (a) >> 16; al_ = (a) & 0xffff; bh_ = (b) >> 16; bl_ = (b) & 0xffff; \
	m1_ = ah_ * (int)bl_; m2_ = (int)al_ * bh_; lo = al_ * bl_; \
	hi = ah_ * bh_ + (m1_ >> 16) + (m2_ >> 16); \
	t_ = lo + ((unsigned)m1_ << 16); hi += t_ < lo; \
	lo = t_ + ((unsigned)m2_ << 16); hi += lo < t_; } while (0)

fixed FixedMul(fixed a, fixed b)
{
	int hi;
	unsigned lo, t;
	C4_MUL64(a, b, hi, lo);
	t = lo + 0x8000; hi += t < lo;
	return (fixed)((t >> 16) | ((unsigned)hi << 16));
}

fixed FixedMulShift(fixed a, fixed b, fixed shift)
{
	int hi;
	unsigned lo;
	__int64 x, y, z;
	if (shift > 0 && shift < 32)
	{
		C4_MUL64(a, b, hi, lo);
		return (fixed)((lo >> shift) | ((unsigned)hi << (32 - shift)));
	}
	x = a; y = b; z = x * y;
	return (((unsigned __int64)z) >> shift) & 0xffffffff;
}
#else
fixed FixedMul(fixed a, fixed b)
{
	__int64 scratch1 = (__int64) a * (__int64) b + (__int64) 0x8000;
	return (scratch1 >> 16) & 0xffffffff;
}

fixed FixedMulShift(fixed a, fixed b, fixed shift)
{
	__int64 x = a;
	__int64 y = b;
	__int64 z = x * y;
	
	return (((unsigned __int64)z) >> shift) & 0xffffffff;
}
#endif

fixed FixedDiv2(fixed a, fixed b)
{
	__int64 x = (signed int)a;
	__int64 y = (signed int)b;
	__int64 z = x * 65536 / y;
	
	return (z) & 0xffffffff;
}

fixed FixedScale(fixed orig, fixed factor, fixed divisor)
{
	__int64 x = orig;
	__int64 y = factor;
	__int64 z = divisor;
	
	__int64 w = (x * y) / z;
	
	return (w) & 0xffffffff;
}
