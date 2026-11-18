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
//******************************************************************************
//
// Private header for RT_STR.C
//
//******************************************************************************

#ifndef _rt_str_private
#define _rt_str_private

//******************************************************************************
//
// PROTOTYPES
//
//******************************************************************************

void VWB_DrawPropString  (const char *string);
void VW_MeasurePropString (const char *string, int *width, int *height );

//void (*USL_MeasureString)(const char *, int *, int *, font_t *) = VW_MeasurePropString,
//     (*USL_DrawString)(const char *) = VWB_DrawPropString;
#ifdef C4ROTT
// c4: the measure is called with four arguments, the font last, and
// VW_MeasurePropString takes three. On x86 the extra one is left over; c4
// pushes the arguments in order, so the function read the width's address
// as the string, and the quit box's message measured millions wide:
// MenuBufCPrintLine's Error ended the game as the box came up.
// VWL_MeasureString is the same measure, the font passed (CurrentFont,
// every caller's).
void VWL_MeasureString (const char *s, int *width, int *height, const font_t *font);
void (*USL_MeasureString)(const char *, int *, int *, font_t *) = (void (*)(const char *, int *, int *, font_t *))VWL_MeasureString,
     (*USL_DrawString)(const char *) = VWB_DrawPropString;
#else
void (*USL_MeasureString)(const char *, int *, int *, font_t *) = (void (*)(const char *, int *, int *, font_t *))VW_MeasurePropString,
     (*USL_DrawString)(const char *) = VWB_DrawPropString;
#endif


#endif
