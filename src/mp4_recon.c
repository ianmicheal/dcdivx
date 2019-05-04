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


#ifdef MIPS

void CopyBlock(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 

__asm(	"addi	$2,$0,+8;" 
		"sub	$5,$5,$6;"
"bucle	:add	$5,$5,$6;" 
		"ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"add	$2,$2,-1;" 
		"add	$4,$4,$6;"
		//"dsrl32	$9,$8,0;" 
		"sw		$8,0($5);" 
		"sw		$9,4($5);"
		
"bgtz	$2,bucle;",Src,Dst,Stride); 

} 

void CopyBlockHor(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 

__asm (	"addi	$2,$0,+8;" 
		"li		$24,0x01010101;"
		"li		$25,0xFEFEFEFE;"
"bucle2	:ulw	$8,0($4);" 
		"ulw	$10,1($4);" 
		"ulw	$9,4($4);" 
		"ulw	$11,5($4);" 
		
		"add	$4,$4,$6;" 
		"addi	$2,$2,-1;" 
	
		"and	$8,$8,$25;"		// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 

		"srl	$8,$8,1;"		//divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 

		"addu	$8,$8,$10;" 
		"addu	$8,$8,$24;"		//we add 1
		"sw		$8,0($5);"		//and we save it
	
		"addu	$9,$9,$11;"
		"addu	$8,$9,$24;"
		"sw		$8,4($5);"	
			
		"add	$5,$5,$6;" 

"bgtz	$2,bucle2;",Src,Dst,Stride);

} 

void CopyBlockVer(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 

__asm (	"addi	$2,$0,+8;" 
		"add	$3,$4,$6;" 
		"li		$24,0x01010101;"
		"li		$25,0xFEFEFEFE;" 
"bucle3 :ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,0($3);" 
		"ulw	$11,4($3);" 

		"add	$4,$4,$6;" 
		"add	$3,$3,$6;" 

		"and	$8,$8,$25;"// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 

		"srl	$8,$8,1;" //divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 

		"addu	$8,$8,$10;" 
		"addu	$8,$8,$24;"	//we add 1
		"sw		$8,0($5);"
		
		"addu	$9,$9,$11;"
		"addu	$8,$9,$24;"
 		"sw		$8,4($5);"
	
		"addi	$2,$2,-1;" 
		"add	$5,$5,$6;" 

"bgtz	$2,bucle3;",Src,Dst,Stride);  

} 

void CopyBlockHorVer(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 

__asm (	"addi	$2,$0,+8;" 
		"add	$3,$4,$6;" 
		"li		$24,0x01010101;"
		"li		$25,0xFEFEFEFE;" 
"bucle4 :ulw	$8,0($4);" 
		"ulw	$10,1($4);"
		"ulw	$9,4($4);" 
		"ulw	$11,5($4);"
		"ulw	$12,0($3);" 
		"ulw	$14,1($3);"
		"ulw	$13,4($3);" 
		"ulw	$15,5($3);"

		"add	$4,$4,$6;" 
		"add	$3,$3,$6;" 

		"and	$8,$8,$25;"// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		"and	$12,$12,$25;"
		"and	$13,$13,$25;"
		"and	$14,$14,$25;"
		"and	$15,$15,$25;"

		"srl	$8,$8,1;" //divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 
		"srl	$12,$12,1;"
		"srl	$13,$13,1;"
		"srl	$14,$14,1;"
		"srl	$15,$15,1;"

		"addu	$8,$8,$12;" 
		"addu	$9,$9,$13;"
		"addu	$10,$10,$14;"
		"addu	$11,$11,$15;"
	
		"and	$8,$8,$25;"// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		
		"srl	$8,$8,1;" //divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 

		"addu	$8,$8,$10;"
		"addu	$8,$8,$24;"//add 1
		"sw		$8,0($5);"
		
		"addu	$9,$9,$11;"
		"addu	$8,$9,$24;"
		"sw		$8,4($5);" 
		
		"addi	$2,$2,-1;" 
		"add	$5,$5,$6;" 

"bgtz	$2,bucle4;",Src,Dst,Stride);  

} 

void CopyBlockHorRound(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 

__asm (	"addi	$2,$0,+8;" 
		"li		$25,0xFEFEFEFE;" 
"bucle5	:ulw	$8,0($4);" 
		"ulw	$10,1($4);"
		"ulw	$9,4($4);" 
		"ulw	$11,5($4);" 

		"addi	$2,$2,-1;" 
		"add	$4,$4,$6;" 

		"and	$8,$8,$25;"// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 

		"srl	$8,$8,1;" //divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 

		"addu	$8,$8,$10;" 
		"sw		$8,0($5);"
		
		"addu	$8,$9,$11;" 
		"sw		$8,4($5);"
		 
 
		"add	$5,$5,$6;" 

"bgtz	$2,bucle5;",Src,Dst,Stride); 


} 

void CopyBlockVerRound(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 
__asm (	"addi	$2,$0,+8;" 
		"add	$3,$4,$6;" 
		"li		$25,0xFEFEFEFE;" 
"bucle6	:ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,0($3);" 
		"ulw	$11,4($3);" 
		
		"add	$4,$4,$6;" 
		"add	$3,$3,$6;" 
		
		"and	$8,$8,$25;" 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 

		"srl	$8,$8,1;"	
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 

		"addu	$8,$8,$10;" 
		"sw		$8,0($5);"
		
		"addu	$8,$9,$11;" 
		"sw		$8,4($5);" 

		"addi	$2,$2,-1;" 
		"add	$5,$5,$6;" 

"bgtz	$2,bucle6;",Src,Dst,Stride); 
} 

void CopyBlockHorVerRound(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 

__asm (	"addi	$2,$0,+8;" 
		"add	$3,$4,$6;" 
		"li		$25,0xFEFEFEFE;" 
		"li		$24,0x01010101;"
"bucle7 :ulw	$8,0($4);" 
		"ulw	$10,1($4);"
		"ulw	$9,4($4);" 
		"ulw	$11,5($4);"
		"ulw	$12,0($3);" 
		"ulw	$14,1($3);"
		"ulw	$13,4($3);" 
		"ulw	$15,5($3);"

		"add	$4,$4,$6;" 
		"add	$3,$3,$6;" 

		"and	$8,$8,$25;"// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		"and	$12,$12,$25;"
		"and	$13,$13,$25;"
		"and	$14,$14,$25;"
		"and	$15,$15,$25;"

		"srl	$8,$8,1;" //divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 
		"srl	$12,$12,1;"
		"srl	$13,$13,1;"
		"srl	$14,$14,1;"
		"srl	$15,$15,1;"

		"addu	$8,$8,$12;" 
		"addu	$8,$8,$24;"
		"addu	$9,$9,$13;"
		"addu	$9,$9,$24;"
		"addu	$10,$10,$14;"
		"addu	$10,$10,$24;"
		"addu	$11,$11,$15;"
		"addu	$11,$11,$24;"
	
		"and	$8,$8,$25;"// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		
		"srl	$8,$8,1;" //divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 

		"addu	$8,$8,$10;"
		"sw		$8,0($5);"
		
		"addu	$8,$9,$11;"
		"sw		$8,4($5);" 
		
		"addi	$2,$2,-1;" 
		"add	$5,$5,$6;" 

"bgtz	$2,bucle7;",Src,Dst,Stride);  
}

static void CopyMBlock(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 
__asm(	"addi	$2,$0,+16;" 
"bucle8	:ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,8($4);" 
		"ulw	$11,12($4);" 
		"add	$2,$2,-1;" 
		"add	$4,$4,$6;" 
		
		"sd		$8,0($5);" 
		"sd		$10,8($5);" 
		"add	$5,$5,$6;" 
"bgtz	$2,bucle8;",Src,Dst,Stride); 
} 

static void CopyMBlockHor(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 
__asm(	"addi	$2,$0,+16;"
		"li		$24,0x01010101;"
		"li		$25,0xFEFEFEFE;"
"bucle9	:ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,8($4);" 
		"ulw	$11,12($4);"
		"ulw	$12,1($4);"
		"ulw	$13,5($4);"
		"ulw	$14,9($4);"
		"ulw	$15,13($4);"
		
		"addi	$2,$2,-1;" 
		"add	$4,$4,$6;" 

		"and	$8,$8,$25;"		// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		"and	$12,$12,$25;"
		"and	$13,$13,$25;"
		"and	$14,$14,$25;"
		"and	$15,$15,$25;"
		
		"srl	$8,$8,1;"		//divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 
		"srl	$12,$12,1;"
		"srl	$13,$13,1;"
		"srl	$14,$14,1;"
		"srl	$15,$15,1;"

		"addu	$8,$8,$12;" 
		"addu	$9,$9,$13;" 
		"addu	$10,$10,$14;"
		"addu	$11,$11,$15;"

		"addu	$8,$8,$24;"	//we add 1
		"sw		$8,0($5);"	
		"addu	$8,$9,$24;"
		"sw		$8,4($5);"
		"addu	$8,$10,$24;"
		"sw		$8,8($5);"
		"addu	$8,$11,$24;"
		"sw		$8,12($5);"
		"add	$5,$5,$6;" 

"bgtz $2,bucle9;",Src,Dst,Stride);
} 

static void CopyMBlockVer(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 
__asm(	"addi	$2,$0,+16;"
		"li		$24,0x01010101;"
		"li		$25,0xFEFEFEFE;"
		"add	$3,$4,$6;" 
		
"bucle10:ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,8($4);" 
		"ulw	$11,12($4);"
		"ulw	$12,0($3);"
		"ulw	$13,4($3);"
		"ulw	$14,8($3);"
		"ulw	$15,12($3);"
		
		"addi	$2,$2,-1;" 
		"add	$4,$4,$6;" 
		"add	$3,$3,$6;"

		"and	$8,$8,$25;"		// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		"and	$12,$12,$25;"
		"and	$13,$13,$25;"
		"and	$14,$14,$25;"
		"and	$15,$15,$25;"
		
		"srl	$8,$8,1;"		//divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 
		"srl	$12,$12,1;"
		"srl	$13,$13,1;"
		"srl	$14,$14,1;"
		"srl	$15,$15,1;"

		"addu	$8,$8,$12;" 
		"addu	$9,$9,$13;" 
		"addu	$10,$10,$14;"
		"addu	$11,$11,$15;"

		"addu	$8,$8,$24;"	//we add 1
		"sw		$8,0($5);"
		"addu	$8,$9,$24;"
		"sw		$8,4($5);"
		"addu	$8,$10,$24;"
		"sw		$8,8($5);"
		"addu	$8,$11,$24;"
		"sw		$8,12($5);"
		"add	$5,$5,$6;" 

"bgtz $2,bucle10;",Src,Dst,Stride);
} 

static void CopyMBlockHorRound(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 
__asm(	"addi	$2,$0,+16;"
		"li		$25,0xFEFEFEFE;"
"bucle12:ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,8($4);" 
		"ulw	$11,12($4);"
		"ulw	$12,1($4);"
		"ulw	$13,5($4);"
		"ulw	$14,9($4);"
		"ulw	$15,13($4);"
		
		"addi	$2,$2,-1;" 
		"add	$4,$4,$6;" 

		"and	$8,$8,$25;"		// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		"and	$12,$12,$25;"
		"and	$13,$13,$25;"
		"and	$14,$14,$25;"
		"and	$15,$15,$25;"
		
		"srl	$8,$8,1;"		//divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 
		"srl	$12,$12,1;"
		"srl	$13,$13,1;"
		"srl	$14,$14,1;"
		"srl	$15,$15,1;"

		"addu	$8,$8,$12;" 
		"sw		$8,0($5);"
		"addu	$8,$9,$13;"
		"sw		$8,4($5);"
		"addu	$8,$10,$14;"
		"sw		$8,8($5);"
		"addu	$8,$11,$15;"
		"sw		$8,12($5);"	

		"add	$5,$5,$6;" 

"bgtz $2,bucle12;",Src,Dst,Stride);
} 

static void CopyMBlockVerRound(unsigned char * Src, unsigned char * Dst, int Stride) 
{ 
__asm(	"addi	$2,$0,+16;"
		"li		$25,0xFEFEFEFE;"
		"add	$3,$4,$6;" 
"bucle13:ulw	$8,0($4);" 
		"ulw	$9,4($4);" 
		"ulw	$10,8($4);" 
		"ulw	$11,12($4);"
		"ulw	$12,0($3);"
		"ulw	$13,4($3);"
		"ulw	$14,8($3);"
		"ulw	$15,12($3);"
		
		"addi	$2,$2,-1;" 
		"add	$4,$4,$6;" 
		"add	$3,$3,$6;"

		"and	$8,$8,$25;"		// in order to divide by two 
		"and	$9,$9,$25;" 
		"and	$10,$10,$25;" 
		"and	$11,$11,$25;" 
		"and	$12,$12,$25;"
		"and	$13,$13,$25;"
		"and	$14,$14,$25;"
		"and	$15,$15,$25;"
		
		"srl	$8,$8,1;"		//divide by two
		"srl	$9,$9,1;" 
		"srl	$10,$10,1;" 
		"srl	$11,$11,1;" 
		"srl	$12,$12,1;"
		"srl	$13,$13,1;"
		"srl	$14,$14,1;"
		"srl	$15,$15,1;"

		"addu	$8,$8,$12;" 
		"sw		$8,0($5);"
		"addu	$8,$9,$13;"
		"sw		$8,4($5);"
		"addu	$8,$10,$14;"
		"sw		$8,8($5);"
		"addu	$8,$11,$15;"
		"sw		$8,12($5);"

		"add	$5,$5,$6;" 

"bgtz $2,bucle13;",Src,Dst,Stride);
} 

#else
#ifndef ARM
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
#endif
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
#ifndef ARM

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
//	}
//	else
//	{
//		Stride>>=2;
//		d=(uint32*)Dst;
//		s=(uint32*)Src;
//		for (dy = 0; dy < 16; dy++) 
//		{
//
//			//printf("Dst=%d SRC=%d\r\n",(int)Dst,(int)Src);
//			*(d++) = *(s++);
//			*(d++) = *(s++);
//			*(d++) = *(s++);
//			*(d++) = *(s++);
//			d += Stride;
//			s += Stride;
//		}
//	}
} 

#endif
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
#endif
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

