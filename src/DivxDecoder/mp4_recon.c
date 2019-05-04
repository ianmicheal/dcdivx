 /*This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The GPL can be found at: http://www.gnu.org/copyleft/gpl.html						*
 *																						*
 **************************************************************************
 *																		  *
 * Copyright (C) 2001 - Project Mayo									  *
 *																		  *
 * Andrea Graziani (Ag)													  *		
 *																		  *	
 * DivX Advanced Research Center <darc@projectmayo.com>					  *		
 *																		  *
 **************************************************************************/
// mp4_recon.c //


#include "mp4_decoder.h"
#include "global.h"


 static  void CopyBlock(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy;
	for (dy = 8; dy ; dy--) 
	{
		Dst[0] = Src[0];
		Dst[1] = Src[1];
		Dst[2] = Src[2];
		Dst[3] = Src[3];
		Dst[4] = Src[4];
		Dst[5] = Src[5];
		Dst[6] = Src[6];
		Dst[7] = Src[7];

		Dst += Stride;
		Src += Stride;
	}
}
/**/
static  void CopyBlockHor(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 8; dy != 0; dy--) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1]+1) >> 1; // hor interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static  void CopyBlockVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 8; dy != 0; dy--) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+Stride] +1) >> 1; // ver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static  void CopyBlockHorVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 8; dy != 0; dy--) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1] + 
								Src[dx+Stride] + Src[dx+Stride+1] +2) >> 2; // horver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyBlockHorRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 8; dy != 0; dy--) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1]) >> 1; // hor interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyBlockVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 8; dy != 0; dy--) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+Stride]) >> 1; // ver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyBlockHorVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 8; dy != 0; dy--) {
		for (dx = 0; dx < 8; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1] + 
								Src[dx+Stride] + Src[dx+Stride+1] +1) >> 2; // horver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/** *** **/
// this routine now in CopyMBlock.obj (asm)
static void CopyMBlock(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy;
//	uint32* d;
//	uint32* s;
	Stride-=16;

//	if (((uint32)Dst)<<30||(((uint32)Src)<<30))
//	{
		for (dy = 16; dy; dy--) 
		{
		
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			*(Dst++) = *(Src++);
			Dst += Stride;
			Src += Stride;
		}
} 

/**/
static void CopyMBlockHor(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 16; dy != 0; dy--) {
		for (dx = 0; dx < 16; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1]+1) >> 1; // hor interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyMBlockVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 16; dy != 0; dy--) {
		for (dx = 0; dx < 16; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+Stride] +1) >> 1; // ver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
static void CopyMBlockHorRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 16; dy != 0; dy--) {
		for (dx = 0; dx < 16; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1]) >> 1; // hor interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyMBlockVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 16; dy != 0; dy--) {
		for (dx = 0; dx < 16; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+Stride]) >> 1; // ver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyMBlockHorVer(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 16; dy != 0; dy--) {
		for (dx = 0; dx < 16; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1] + 
								Src[dx+Stride] + Src[dx+Stride+1] +2) >> 2; // horver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}
/**/
static void CopyMBlockHorVerRound(unsigned char * Src, unsigned char * Dst, int Stride)
{
	int dy, dx;

	for (dy = 16; dy != 0; dy--) {
		for (dx = 0; dx < 16; dx++) {
			Dst[dx] = (Src[dx] + Src[dx+1] + 
								Src[dx+Stride] + Src[dx+Stride+1] +1) >> 2; // horver interpolation with rounding
		}
		Src += Stride;
		Dst += Stride;
	}
}


/**/

void recon_comp (unsigned char *src, unsigned char *dst, int lx, int w, int x, int y, int dx, int dy);

/**/

void reconstruct (int bx, int by, int mode)
{
	int dx, dy, comp, sum;
	int x, y, px, py;
		
	x = bx + 1;
	y = by + 1;

  	// Luma
	
	px = bx << 4;
	py = by << 4;
	if (mode == INTER4V)
	{
		for (comp = 0; comp < 4; comp++)
		{
			dx = MV[x][y][comp][0];
			dy = MV[x][y][comp][1];
			
			recon_comp (frame_for[0], frame_ref[0], coded_picture_width, 8, px + ((comp & 1) << 3), py + ((comp & 2) << 2), dx, dy);
		}
	} else
	{
		dx = MV[x][y][0][0];
		dy = MV[x][y][0][1];

		recon_comp (frame_for[0], frame_ref[0], coded_picture_width, 16, px, py, dx, dy);
	}
	// Chr
	px = bx << 3;
	py = by << 3;
	if (mode == INTER4V)
	{
		sum = MV[x][y][0][0] + MV[x][y][1][0] + MV[x][y][2][0] + MV[x][y][3][0];
		if (sum == 0) 
			dx = 0;
		else
			dx = sign (sum) * (roundtab[abs (sum) & 0xF] + ((abs (sum) >> 4) <<1));
		
		sum = MV[x][y][0][1] + MV[x][y][1][1] + MV[x][y][2][1] + MV[x][y][3][1];
		if (sum == 0)
			dy = 0;
		else
			dy = sign (sum) * (roundtab[abs (sum) & 0xF] + ((abs (sum) >> 4) <<1));
	} else
	{
		dx = MV[x][y][0][0];
		dy = MV[x][y][0][1];
		// chroma rounding 
		dx = ((dx & 0x3) == 0 ? dx >> 1 : (dx >> 1) | 1);
		dy = ((dy & 0x3) == 0 ? dy >> 1 : (dy >> 1) | 1);
	}

	recon_comp (frame_for[1], frame_ref[1], coded_picture_width>>1, 8, px, py, dx, dy);
	recon_comp (frame_for[2], frame_ref[2], coded_picture_width>>1, 8, px, py, dx, dy);
}

/**/

static void recon_comp (unsigned char *src, unsigned char *dst, 
                        int lx, int w, int x, 
                        int y, int dx, int dy)
{
	unsigned char *s, *d;
	int mc_driver;

	// origins 
	s = src + lx * (y + (dy >> 1)) + x + (dx >> 1);
	d = dst + lx * y + x;

	mc_driver = ((w!=8)<<3) | (mp4_hdr.rounding_type<<2) | ((dy & 1)<<1) | (dx & 1);
	switch (mc_driver)
		{
			// block
		   // no round
			case 0: case 4:
 			  CopyBlock(s, d, lx);
			  break;
			case 1:
				CopyBlockHor(s, d, lx);
				break;
			case 2:
				CopyBlockVer(s, d, lx);
				break;
			case 3:
				CopyBlockHorVer(s, d, lx);
				break;
				// round
			case 5:
				CopyBlockHorRound(s, d, lx);
				break;
			case 6:
				CopyBlockVerRound(s, d, lx);
				break;
			case 7:
				CopyBlockHorVerRound(s, d, lx);
				break;
		// macroblock
			// no round
			case 8: case 12:
				CopyMBlock(s, d, lx);
				break;
			case 9:
				CopyMBlockHor(s, d, lx);
				break;
			case 10:
				CopyMBlockVer(s, d, lx);
				break;
			case 11:
				CopyMBlockHorVer(s, d, lx);
				break;
			// round
			case 13:
				CopyMBlockHorRound(s, d, lx);
				break;
			case 14:
				CopyMBlockVerRound(s, d, lx);
				break;
			case 15:
				CopyMBlockHorVerRound(s, d, lx);
				break;
		}
}

