/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef _watcom_h_public
#define _watcom_h_public
fixed FixedMul(fixed a, fixed b);
fixed FixedDiv2(fixed a, fixed b);
fixed FixedScale(fixed orig, fixed factor, fixed divisor);
fixed FixedMulShift(fixed a, fixed b, fixed shift);
#ifdef C4ROTT
#include "c4accel.h"     /* c4: the accelerators, rott_accel */
#endif
#if defined(C4ROTT) && defined(__c4_inttypes__)
/* c4: with the accelerators (c4/c4accel.h), each of these is their one
   instruction where it stands: c4lc makes every function a call, and the
   call and the 64-bit arithmetic cost more than the product (FixedMul 1,100
   calls a frame of demo 4, FixedScale and FixedDiv2 through the __int64
   library). Each argument is used once on either side; a shift is always a
   constant and a divisor a plain value, tested and then used. Otherwise,
   and until the game has asked (rott_accel -1), the function, which
   watcom.c names c4_FixedMul and so on here. */
fixed c4_FixedMul(fixed a, fixed b);
fixed c4_FixedDiv2(fixed a, fixed b);
fixed c4_FixedScale(fixed orig, fixed factor, fixed divisor);
fixed c4_FixedMulShift(fixed a, fixed b, fixed shift);
#define FixedMul(a, b) (rott_accel > 0 ? rott_fixmul((a), (b)) : c4_FixedMul((a), (b)))
#define FixedMulShift(a, b, s) (rott_accel > 0 && (s) > 0 && (s) < 32 ? \
	rott_fixmulshift((a), (b), (s)) : c4_FixedMulShift((a), (b), (s)))
#define FixedDiv2(a, b) (rott_accel > 0 && (b) != 0 ? \
	rott_fixdiv2((a), (b)) : c4_FixedDiv2((a), (b)))
#define FixedScale(a, b, c) (rott_accel > 0 && (c) != 0 ? \
	rott_fixscale((a), (b), (c)) : c4_FixedScale((a), (b), (c)))
#endif
#ifdef __WATCOMC__
#pragma aux FixedMul =  \
        "imul ebx",                     \
        "add  eax, 8000h"        \
        "adc  edx,0h"            \
        "shrd eax,edx,16"       \
        parm    [eax] [ebx] \
        value   [eax]           \
        modify exact [eax edx]

#pragma aux FixedMulShift =  \
        "imul ebx",                     \
        "shrd eax,edx,cl"       \
        parm    [eax] [ebx] [ecx]\
        value   [eax]           \
        modify exact [eax edx]

#pragma aux FixedDiv2 = \
        "cdq",                          \
        "shld edx,eax,16",      \
        "sal eax,16",           \
        "idiv ebx"                      \
        parm    [eax] [ebx] \
        value   [eax]           \
        modify exact [eax edx]
#pragma aux FixedScale = \
        "imul ebx",                     \
        "idiv ecx"                      \
        parm    [eax] [ebx] [ecx]\
        value   [eax]           \
        modify exact [eax edx]
#endif

#endif
