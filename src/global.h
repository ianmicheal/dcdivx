 /*This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The GPL can be found at: http://www.gnu.org/copyleft/gpl.html						*
 *																						*
 *																						*	
 **************************************************************************************
 *                                                                        *    
 * Copyright (C) 2001 - Project Mayo                                      *
 *                                                                        *
 * Andrea Graziani                                                        *                                                         
 *                                                                        *
 * DivX Advanced Research Center <darc@projectmayo.com>                   * 
 *                                                                        *   
 **************************************************************************/

// global.h //

/* GLOBAL is defined in only one file */

#include <stdio.h>
#include "portab.h"

#ifndef GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

//
//	macros 
//

#define mmax(a, b)        ((a) > (b) ? (a) : (b))
#define mmin(a, b)        ((a) < (b) ? (a) : (b))
#define mnint(a)		  ((a) < 0 ? (int)(a - 0.5) : (int)(a + 0.5))
#define sign(a)			  ((a) < 0 ? -1 : 1)
int abs( int n );
double ceil( double x );
double log( double x );

// 
// 	prototypes of global functions
//

// getbits.c 

void initbits (void);
unsigned int showbits (int n);
void flushbits (int n);
unsigned int getbits (int n);
unsigned int getbits1(void);

// idct.c 

void idct (idct_block_t *block);

// recon.c 

void reconstruct (int bx, int by, int mode);

// mp4_picture.c

void PictureDisplay (unsigned char *bmp, int render_flag,int Extra);

// 
//		global variables 
//

// zig-zag scan

const unsigned char zig_zag_scan[64]
#ifdef GLOBAL
=
{
  0, 1, 8, 16, 9, 2, 3, 10, 17,
  24, 32, 25, 18, 11, 4, 5, 12, 
  19, 26, 33, 40, 48, 41, 34, 27,
  20, 13, 6, 7, 14, 21, 28, 35, 42,
  49, 56, 57, 50, 43, 36, 29, 22,
  15, 23, 30, 37, 44, 51, 58, 59, 
  52, 45, 38, 31, 39, 46, 53, 60, 61,
  54, 47, 55, 62, 63
}
#endif
;

// other scan orders
const unsigned char alternate_horizontal_scan[64]
#ifdef GLOBAL
=
{
   0,  1,  2,  3,  8,  9, 16, 17, 
	10, 11,  4,  5,  6,  7, 15, 14,
  13, 12, 19, 18, 24, 25, 32, 33, 
	26, 27, 20, 21, 22, 23, 28, 29,
  30, 31, 34, 35, 40, 41, 48, 49, 
	42, 43, 36, 37, 38, 39, 44, 45,
  46, 47, 50, 51, 56, 57, 58, 59, 
	52, 53, 54, 55, 60, 61, 62, 63
}
#endif
;

const unsigned char alternate_vertical_scan[64]
#ifdef GLOBAL
=
{
   0,  8, 16, 24,  1,  9,  2, 10, 
	17, 25, 32, 40, 48, 56, 57, 49,
  41, 33, 26, 18,  3, 11,  4, 12, 
	19, 27, 34, 42, 50, 58, 35, 43,
  51, 59, 20, 28,  5, 13,  6, 14, 
	21, 29, 36, 44, 52, 60, 37, 45,
  53, 61, 22, 30,  7, 15, 23, 31, 
	38, 46, 54, 62, 39, 47, 55, 63
}
#endif
;

EXTERN int coded_picture_width, coded_picture_height;

const unsigned char roundtab[16]
#ifdef GLOBAL
= {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2}
#endif
;

//
//	mp4 stuff
//

#include "mp4_header.h"

EXTERN int juice_flag;
EXTERN mp4_header mp4_hdr;

EXTERN int MV[32][32][4][2];  
EXTERN short modemap[32][32];


EXTERN struct _base
{
 	// DCT BLOCK

	idct_block_t block[64];
	
	// DATA BUFFER
	
	int bitcnt;
	unsigned char *rdptr;
  
} base, *ld;

EXTERN struct _ac_dc
{
	int predict_dir;
	int ac_dc_left_lum[64][64][8];
	int ac_dc_top_lum[64][64][7];

	int ac_dc_left_chr[2][32][32][8];
	int ac_dc_top_chr[2][32][32][7];


} ac_dc, *coeff_pred;
EXTERN unsigned char	*frame_ref[3],*frame_for[3],*end_ref[3];
EXTERN char * tempBB;
