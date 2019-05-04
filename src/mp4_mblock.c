#if !defined(MIPS) || (_WIN32_WCE >= 300)
 /*This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The GPL can be found at: http://www.gnu.org/copyleft/gpl.html						*
 *																						*
 **************************************************************************
 *                                                                        *      
 * Copyright (C) 2001 - Project Mayo                                      *
 *                                                                        *  
 * Andrea Graziani (Ag)                                                   *
 *                                                                        *   
 * DivX Advanced Research Center <darc@projectmayo.com>                   *
 *																		  *	
 **************************************************************************/

// mp4_mblock.c //

#include "mp4_decoder.h"
#include "global.h"
#include "mp4_mblock.h"
#include "mp4_predict.h"
#include "mp4_vld.h"
#include "portab.h"

void clearblock(idct_block_t * psblock);
void idct_special (idct_block_t *block, uint8_t *destU8, int stride,int flag);
void idctrow_special (idct_block_t *blk,uint8_t *destU8,int flag);
void idctcol (idct_block_t *blk);


#define W1 2841                 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676                 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408                 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609                 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108                 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565                  /* 2048*sqrt(2)*cos(7*pi/16) */

#define W1_minus_W7 2276
#define W1_plus_W7 3406
#define W3_minus_W5 799
#define W3_plus_W5 4017
#define W2_minus_W6 1568
#define W2_plus_W6 3784

#ifdef MIPS
#ifndef	MIPS32
#define MIPS_MACC
#endif
#endif

/* row (horizontal) IDCT
 * 
 * 7                       pi         1 dst[k] = sum c[l] * src[l] * cos( -- *
 * ( k + - ) * l ) l=0                      8          2
 * 
 * where: c[0]    = 128 c[1..7] = 128*sqrt(2)								*/

#ifdef MIPS_MACC

//Inverse Discrete Cosinus Transform(IDCT)
//MIPS ASM - MACC VERSION 
//v0.1  => first working version. No optimizations at all 
//v0.2a => it´s macc optimized.
//v0.3	=> 8 bits destination buffer-bypass buffer conversions.
//v0.4	=> Merged both idctrows functions of v0.3

 void idctrow_special (idct_block_t *blk, uint8_t *destU8,int flag )
{
  __asm("add	$24,$5,$0;"
		"add	$25,$6,$0;"
		"lh		$5,0($4);"		//x0 
		"lh		$9,2($4);"		//x4 
		"lh		$8,4($4);"		//x3 
		"lh		$12,6($4);"		//x7 
		"lh		$6,8($4);"		//x1 
		"lh		$11,10($4);"	//x6 
		"lh		$7,12($4);"		//x2 
		"lh		$10,14($4);"	//x5 

		"sll	$6,$6,8;"		// x1<< 

		"or		$2,$7,$6;" 
		"or		$2,$2,$8;" 
		"or		$2,$2,$9;" 
		"or		$2,$2,$10;" 
		"or		$2,$2,$11;" 
		"or		$2,$2,$12;"		//shortcut 
		 
		"bne $2,$0,idct_row_a;"	//if some values aren´t 0, when can´t shorcut 
			"addi	$5,$5,32;"
			"sra	$5,$5,6;"
			"bne	$25,$0,add_eq;"
			
			"srl	$15,$5,16;"	//if less than 0, then 0
			"srlv	$5,$5,$15;"

			"sll	$15,$5,23;"	//if bigger than 255:255
			"sra	$15,$15,31;"
			"or		$5,$5,$15;"
			
			"andi	$5,$5,0x00FF;"
			"sll	$14,$5,8;"
			"or		$15,$5,$14;"
			"sll	$14,$5,16;"
			"or		$15,$15,$14;"
			"sll	$14,$5,24;"
			"or		$5,$15,$14;"
			
			"sw		$5,0($24);"	//guardamos x0 
			"sw		$5,4($24);" 
			"jr		$31;");
						
__asm(		"add_eq:lbu	$6,0($24);"	//we load older values
			"lbu	$7,1($24);"	//
			"lbu	$8,2($24);"	//	
			"lbu	$9,3($24);"	//
			"lbu	$10,4($24);"//
			"lbu	$11,5($24);"//
			"lbu	$12,6($24);"//
			"lbu	$13,7($24);"//

			"add	$14,$5,$6;"

			"srl	$15,$14,16;"	//if less than 0, then 0
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	//if bigger than 255:255
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,0($24);"	//and we save it

			"add	$14,$5,$7;"		//we do the same with next ones

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,1($24);"	
			
			"add	$14,$5,$8;"		

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,2($24);"

			"add	$14,$5,$9;"		

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,3($24);"
			);

__asm(		"add	$14,$5,$10;"		

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,4($24);"

			"add	$14,$5,$11;"		

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,5($24);"

			"add	$14,$5,$12;"		

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,6($24);"

			"add	$14,$5,$13;"		

			"srl	$15,$14,16;"	
			"srlv	$14,$14,$15;"

			"sll	$15,$14,23;"	
			"sra	$15,$15,31;"
			"or		$14,$14,$15;"
			"sb		$14,7($24);"//and finish

			"jr		$31;",blk); 
	
__asm  ("idct_row_a:add	$13,$9,$10;"//x8=x4+x5
		"li		$14,565;"		//W7
		"mult	$14,$13;"		//W7*x8
		"li		$15,2276;"		//W1_minus_W7 
		"macc	$0,$15,$9;"		//x4=x(W1-W7)*x4+W7*8
		"mflo	$9;"

		"sll	$5,$5,8;"		// x0<<8)
		
		"mult	$14,$13;"		//W7*x8
		"li		$15,-3406;"		//-W1_plus_W7
		"macc	$0,$15,$10;"	//-((W1+W7)*x5)+W7*x8
		"mflo	$10;"
		
		"li		$14,2408;"		//W3
		"add	$13,$11,$12;"	// x6+x7
		"mult	$14,$13;"		//W3*(x6+x7)

		"li		$15,-799;"		//-W3_minus_W5
		"macc	$0,$15,$11;"	//-((W3-W5)*x6)+W3*(x6+x7)
		"mflo	$11;"

		"add	$5,$5,8192;"	//x0+=128

		"mult	$14,$13;"
		"li		$14,-4017;"
		"macc	$0,$12,$14;"
		"mflo	$12;"
		);		

__asm (	"add	$13,$5,$6;" 
		"sub	$5,$5,$6;"		//x0 -=x1 
		
		"add	$6,$7,$8;"		//x1=(x2+x3)
		"li		$15,1108;"		//W6 
		"mult	$15,$6;"		//W6*(x3+x2) 
		
		"li		$14,-3784;"		//-W2_plus_W6 
		"macc	$0,$14,$7;"		//x1+(-W2_plus_W6*x2) 
		"mflo	$7;"
		"mult	$15,$6;"		//W6*(x3+x2)
		"li		$14,1568;"		//W2_minus_W6 
		"macc	$0,$14,$8;"		//W2_minus_W6*x3 
		"mflo	$8;" 

		"addi	$9,$9,4;"
		"addi	$11,$11,4;"
		"addi	$10,$10,4;"
		"addi	$12,$12,4;"

		"sra	$9,$9,3;"
		"sra	$10,$10,3;"
		"sra	$11,$11,3;"
		"sra	$12,$12,3;"
		
		"add	$6,$9,$11;"		//x1=x4+x6 
		"sub	$9,$9,$11;"		//x4=x4-x6 
		"add	$11,$10,$12;"	//x6=x5+x7 
		"sub	$10,$10,$12;"	//x5=x5-x7 
		
); 

__asm (	"addi	$7,$7,4;"
		"addi	$8,$8,4;"
		"sra	$7,$7,3;"
		"sra	$8,$8,3;"
		"add	$12,$13,$8;"	//x7=x8+x3 
		"sub	$13,$13,$8;"	//x8-=x3 
		"add	$8,$5,$7;"		//x3=x0+x2 
		"sub	$5,$5,$7;"		//x0-=x2 

		"li		$14,181;" 
		"add	$7,$9,$10;"		//x4+x5 
		"mult	$14,$7;"		// 
		"mflo	$7;" 
		"addi	$7,$7,128;" 
		"sra	$7,$7,8;" 

		"sub	$9,$9,$10;" 
		"mult	$14,$9;" 
		"mflo	$9;" 
		"addi	$9,$9,128;" 
		"sra	$9,$9,8;"); 

//Fourth Stage 


__asm (	"bne	$25,$0,add;"
		"add	$15,$12,$6;"	//x7+x1 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//if less than 0, then 0
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,0($24);" 

		"add	$15,$7,$8;"		//x3+x2 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,1($24);" 

		"add	$15,$5,$9;"		//x0+x4 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,2($24);" 

		"add	$15,$13,$11;"	//x8+x6 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,3($24);" 

		"sub	$15,$13,$11;" 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,4($24);" 

		"sub	$15,$5,$9;" 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,5($24);" 

		"sub	$15,$8,$7;" 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,6($24);" 

		"sub	$15,$12,$6;" 
		"sra	$15,$15,14;" 
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,7($24);"
		"jr		$31;");

__asm (	"add:add	$15,$12,$6;"	//x7+x1 
		"lbu	$2,0($24);"
		"sra	$15,$15,14;" 
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//if less than 0, then 0
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,0($24);" 

		"add	$15,$7,$8;"		//x3+x2 
		"lbu	$2,1($24);"
		"sra	$15,$15,14;" 
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,1($24);" 

		"add	$15,$5,$9;"		//x0+x4 
		"lbu	$2,2($24);"
		"sra	$15,$15,14;"
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,2($24);" 

		"add	$15,$13,$11;"	//x8+x6 
		"lbu	$2,3($24);"
		"sra	$15,$15,14;"
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,3($24);"
		);

__asm(	"sub	$15,$13,$11;"
		"lbu	$2,4($24);"
		"sra	$15,$15,14;"
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,4($24);" 

		"sub	$15,$5,$9;" 
		"lbu	$2,5($24);"
		"sra	$15,$15,14;"
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,5($24);" 

		"sub	$15,$8,$7;" 
		"lbu	$2,6($24);"
		"sra	$15,$15,14;"
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,6($24);" 

		"sub	$15,$12,$6;"
		"lbu	$2,7($24);"
		"sra	$15,$15,14;" 
		"add	$15,$15,$2;"
		"srl	$14,$15,16;"	//
		"srlv	$15,$15,$14;"	//
		"sll	$14,$15,23;"	//if bigger than 255:255
		"sra	$14,$14,31;"	//
		"or		$15,$15,$14;"	//
		"sb		$15,7($24);"
		); 
}

 void idctcol (idct_block_t *blk) 
{ 

__asm (	"lh		$5,0($4);"//x0 
		"lh		$9,16($4);"//x4 
		"lh		$8,32($4);"//x3 
		"lh		$12,48($4);"//x7 
		"lh		$6,64($4);"//x1 
		"lh		$11,80($4);"//x6 
		"lh		$7,96($4);"//x2 
		"lh		$10,112($4);"//x5 

		"or		$2,$7,$6;" 
		"or		$2,$2,$8;" 
		"or		$2,$2,$9;" 
		"or		$2,$2,$10;" 
		"or		$2,$2,$11;" 
		"or		$2,$2,$12;"//shortcut 

		"sll	$6,$6,11;"// x1<<11 
		
		"bne $2,$0,idct_estandar;"//si no es 0 no podemos atajar 
			"sll	$5,$5,3;"//x0<<3 
			"beq	$5,$0,final;"
			
			"sh		$5,0($4);"//guardamos x0 
			"sh		$5,16($4);" 
			"sh		$5,32($4);"
			"sh		$5,48($4);"
			"sh		$5,64($4);" 
			"sh		$5,80($4);"
			"sh		$5,96($4);"
			"sh		$5,112($4);" 
			"final:jr		$31;",blk); 
	
__asm  ("idct_estandar:add	$13,$9,$10;"//x8=x4+x5
		"li		$14,565;"//W7
		"mult	$14,$13;"//W7*x8
		"li		$15,2276;"//W1_minus_W7 
		"macc	$0,$15,$9;"//x4=x(W1-W7)*x4+W7*8
		"mflo	$9;"

		"sll	$5,$5,11;"// x0<<11
		
		"mult	$14,$13;"//W7*x8
		"li		$15,-3406;"	//-W1_plus_W7
		"macc	$0,$15,$10;"//-((W1+W7)*x5)+W7*x8
		"mflo	$10;"
		
		"li		$14,2408;"//W3
		"add	$13,$11,$12;"// x6+x7
		"mult	$14,$13;"//W3*(x6+x7)

		"li		$15,-799;"//-W3_minus_W5
		"macc	$0,$15,$11;"//-((W3-W5)*x6)+W3*(x6+x7)
		"mflo	$11;"

		"add	$5,$5,128;"//x0+=128

		"mult	$14,$13;"
		"li		$14,-4017;"
		"macc	$0,$12,$14;"
		"mflo	$12;"
		);		

__asm (	"add	$13,$5,$6;" 
		"sub	$5,$5,$6;"//x0 -=x1 
		
		"add	$6,$7,$8;"//x1=(x2+x3)
		"li		$15,1108;"//W6 
		"mult	$15,$6;"//W6*(x3+x2) 
		
		"li		$14,-3784;" //-W2_plus_W6 
		"macc	$0,$14,$7;" //x1+(-W2_plus_W6*x2) 
		"mflo	$7;"
	
		"mult	$15,$6;"//W6*(x3+x2)
		"li		$14,1568;"//W2_minus_W6 
		"macc	$0,$14,$8;"//W2_minus_W6*x3 
		"mflo	$8;" 

		"add	$6,$9,$11;"//x1=x4+x6 
		"sub	$9,$9,$11;"//x4=x4-x6 
		"add	$11,$10,$12;"//x6=x5+x7 
		"sub	$10,$10,$12;"//x5=x5-x7 
		
); 

__asm (	"add	$12,$13,$8;"//x7=x8+x3 
		"sub	$13,$13,$8;"//x8-=x3 
		"add	$8,$5,$7;"//x3=x0+x2 
		"sub	$5,$5,$7;"//x0-=x2 

		"li		$14,181;" 
		"add	$7,$9,$10;"//x4+x5 
		"mult	$14,$7;"// 
		"mflo	$7;" 
		"addi	$7,$7,128;" 
		"sra	$7,$7,8;" 

		"sub	$9,$9,$10;" 
		"mult	$14,$9;" 
		"mflo	$9;" 
		"addi	$9,$9,128;" 
		"sra	$9,$9,8;"); 

//Fourth Stage 

__asm (	"add	$24,$12,$6;"//x7+x1 
		"sra	$24,$24,8;" 
		"sh		$24,0($4);" 

		"add	$24,$7,$8;"//x3+x2 
		"sra	$24,$24,8;" 
		"sh		$24,16($4);" 

		"add	$24,$5,$9;"//x0+x4 
		"sra	$24,$24,8;" 
		"sh		$24,32($4);" 

		"add	$24,$13,$11;"//x8+x6 
		"sra	$24,$24,8;" 
		"sh		$24,48($4);" 

		"sub	$24,$13,$11;" 
		"sra	$24,$24,8;" 
		"sh		$24,64($4);" 

		"sub	$24,$5,$9;" 
		"sra	$24,$24,8;" 
		"sh		$24,80($4);" 

		"sub	$24,$8,$7;" 
		"sra	$24,$24,8;" 
		"sh		$24,96($4);" 

		"sub	$24,$12,$6;" 
		"sra	$24,$24,8;" 
		"sh		$24,112($4);"); 
} 
#else

void idctcol (idct_block_t *blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;

	x4 = blk[8];
	x3 = blk[16];
	x7 = blk[24];
	x1 = blk[32] << 11;
	x6 = blk[40];
	x2 = blk[48];	
	x5 = blk[56];
	
  if (!((x1) | (x2) | (x3) | (x4) | (x5) | (x6) | (x7)))
  {
    blk[0] = blk[8] = blk[16] = blk[24] = blk[32] = blk[40] = blk[48] = blk[56] = blk[0] << 3;
    return;
  }
  x0 = (blk[0] << 11) + 128;    
  x8 = W7 * (x4 + x5);
  x4 = x8 + (W1_minus_W7) * x4;
  x5 = x8 - (W1_plus_W7) * x5;
  x8 = W3 * (x6 + x7);
  x6 = x8 - (W3_minus_W5) * x6;
  x7 = x8 - (W3_plus_W5) * x7;

  x8 = x0 + x1;
  x0 -= x1;
  x1 = W6 * (x3 + x2);
  x2 = x1 - (W2_plus_W6) * x2;
  x3 = x1 + (W2_minus_W6) * x3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;

  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181 * (x4 + x5) + 128) >> 8;
  x4 = (181 * (x4 - x5) + 128) >> 8;

  blk[0] = (idct_block_t)((x7 + x1) >> 8);
  blk[8] = (idct_block_t)((x3 + x2) >> 8);
  blk[16] = (idct_block_t)((x0 + x4) >> 8);
  blk[24] = (idct_block_t)((x8 + x6) >> 8);
  blk[32] = (idct_block_t)((x8 - x6) >> 8);
  blk[40] = (idct_block_t)((x0 - x4) >> 8);
  blk[48] = (idct_block_t)((x3 - x2) >> 8);
  blk[56] = (idct_block_t)((x7 - x1) >> 8);

}
void idctrow_special (idct_block_t *blk, uint8_t *destU8,int flag)
{
  
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;
  int temp;
	x4 = blk[1];
  	x3 = blk[2];
  	x7 = blk[3];
	x1 = (blk[4] << 8);
	x6 = blk[5];
	x2 = blk[6];
	x5 = blk[7];
	
  if (!((x1) | (x2) | (x3) | (x4) | (x5) | (x6) | (x7)))
  {
    
	temp =  (blk[0] + 32) >> 6; 
	
	if (flag==1) {
		destU8[0] = (temp+destU8[0])>255?255:(temp+destU8[0])<0?0:(temp+destU8[0]);
		destU8[1] = (temp+destU8[1])>255?255:(temp+destU8[1])<0?0:(temp+destU8[1]);
		destU8[2] = (temp+destU8[2])>255?255:(temp+destU8[2])<0?0:(temp+destU8[2]);
		destU8[3] = (temp+destU8[3])>255?255:(temp+destU8[3])<0?0:(temp+destU8[3]);
		destU8[4] = (temp+destU8[4])>255?255:(temp+destU8[4])<0?0:(temp+destU8[4]);
		destU8[5] = (temp+destU8[5])>255?255:(temp+destU8[5])<0?0:(temp+destU8[5]);
		destU8[6] = (temp+destU8[6])>255?255:(temp+destU8[6])<0?0:(temp+destU8[6]);
		destU8[7] = (temp+destU8[7])>255?255:(temp+destU8[7])<0?0:(temp+destU8[7]);
	}
	else	{
		temp=temp>255? 255: temp<0?0:temp;
		destU8[0]=destU8[1]=destU8[2]=destU8[3]=destU8[4]=destU8[5]=destU8[6]=destU8[7]=temp;	
	}
   
	return;
  }
  x0 = (blk[0] << 8) + 8192;

  x8 = W7 * (x4 + x5) + 4;
  x4 = (x8 + (W1_minus_W7) * x4) >> 3;
  x5 = (x8 - (W1_plus_W7) * x5) >> 3;
  x8 = W3 * (x6 + x7) + 4;
  x6 = (x8 - (W3_minus_W5) * x6) >> 3;
  x7 = (x8 - (W3_plus_W5) * x7) >> 3;

  x8 = x0 + x1;
  x0 -= x1;
  x1 = W6 * (x3 + x2) + 4;
  x2 = (x1 - (W2_plus_W6) * x2) >> 3;
  x3 = (x1 + (W2_minus_W6) * x3) >> 3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;

  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181 * (x4 + x5) + 128) >> 8;
  x4 = (181 * (x4 - x5) + 128) >> 8;
  if (flag ==1){
	  destU8[0] = (((x7 + x1) >> 14)+destU8[0])>255? 255: (((x7 + x1) >> 14)+destU8[0])<0 ? 0:(((x7 + x1) >> 14)+destU8[0]);
	  destU8[1] = (((x3 + x2) >> 14)+destU8[1])>255? 255: (((x3 + x2) >> 14)+destU8[1])<0 ? 0:(((x3 + x2) >> 14)+destU8[1]);
	  destU8[2] = (((x0 + x4) >> 14)+destU8[2])>255? 255: (((x0 + x4) >> 14)+destU8[2])<0 ? 0:(((x0 + x4) >> 14)+destU8[2]);
	  destU8[3] = (((x8 + x6) >> 14)+destU8[3])>255? 255: (((x8 + x6) >> 14)+destU8[3])<0 ? 0:(((x8 + x6) >> 14)+destU8[3]);
	  destU8[4] = (((x8 - x6) >> 14)+destU8[4])>255? 255: (((x8 - x6) >> 14)+destU8[4])<0 ? 0:(((x8 - x6) >> 14)+destU8[4]);
	  destU8[5] = (((x0 - x4) >> 14)+destU8[5])>255? 255: (((x0 - x4) >> 14)+destU8[5])<0 ? 0:(((x0 - x4) >> 14)+destU8[5]);
	  destU8[6] = (((x3 - x2) >> 14)+destU8[6])>255? 255: (((x3 - x2) >> 14)+destU8[6])<0 ? 0:(((x3 - x2) >> 14)+destU8[6]);
	  destU8[7] = (((x7 - x1) >> 14)+destU8[7])>255? 255: (((x7 - x1) >> 14)+destU8[7])<0 ? 0:(((x7 - x1) >> 14)+destU8[7]);}
  else{
	  destU8[0] = ((x7 + x1) >> 14)>255? 255: ((x7 + x1) >> 14)<0 ? 0:((x7 + x1) >> 14);
	  destU8[1] = ((x3 + x2) >> 14)>255? 255: ((x3 + x2) >> 14)<0 ? 0:((x3 + x2) >> 14);
	  destU8[2] = ((x0 + x4) >> 14)>255? 255: ((x0 + x4) >> 14)<0 ? 0:((x0 + x4) >> 14);
	  destU8[3] = ((x8 + x6) >> 14)>255? 255: ((x8 + x6) >> 14)<0 ? 0:((x8 + x6) >> 14);
	  destU8[4] = ((x8 - x6) >> 14)>255? 255: ((x8 - x6) >> 14)<0 ? 0:((x8 - x6) >> 14);
	  destU8[5] = ((x0 - x4) >> 14)>255? 255: ((x0 - x4) >> 14)<0 ? 0:((x0 - x4) >> 14);
	  destU8[6] = ((x3 - x2) >> 14)>255? 255: ((x3 - x2) >> 14)<0 ? 0:((x3 - x2) >> 14);
	  destU8[7] = ((x7 - x1) >> 14)>255? 255: ((x7 - x1) >> 14)<0 ? 0:((x7 - x1) >> 14);
	  }
 }

#endif
/* two dimensional inverse discrete cosine transform */ 
void idct_special (idct_block_t *block, uint8_t *destU8, int stride, int flag)
{

	idctcol(block);
	idctcol(block+1);
	idctcol(block+2);
	idctcol(block+3);
	idctcol(block+4);
	idctcol(block+5);
	idctcol(block+6);
	idctcol(block+7);	
	
	idctrow_special(block,destU8,flag);
	destU8+=stride;
	idctrow_special(block+8,destU8,flag);
	destU8+=stride;
	idctrow_special(block+16,destU8,flag);
	destU8+=stride;
	idctrow_special(block+24,destU8,flag);
	destU8+=stride;
	idctrow_special(block+32,destU8,flag);
	destU8+=stride;
	idctrow_special(block+40,destU8,flag);
	destU8+=stride;
	idctrow_special(block+48,destU8,flag);
	destU8+=stride;
	idctrow_special(block+56,destU8,flag);
	clearblock(block);	


}

//end IDCT
/**/

int getDCsizeLum();
int getDCsizeChr();
int getDCdiff();
void setDCscaler(int block_num);

/**/

int blockIntra(int block_num, int coded)
{
	int i;
	int dct_dc_size, dct_dc_diff;
	event_t event;

	// dc coeff
	setDCscaler(block_num); // calculate DC scaler

	if (block_num < 4) {
		dct_dc_size = getDCsizeLum();
		if (dct_dc_size != 0) 
			dct_dc_diff = getDCdiff(dct_dc_size);
		else 
			dct_dc_diff = 0;
		if (dct_dc_size > 8)
			flushbits(1); // marker bit
	}
	else {
		dct_dc_size = getDCsizeChr();
		if (dct_dc_size != 0)
			dct_dc_diff = getDCdiff(dct_dc_size);
		else 
			dct_dc_diff = 0;
		if (dct_dc_size > 8)
			flushbits(1); // marker bit
	}

	ld->block[0] = (idct_block_t) dct_dc_diff;

	// dc reconstruction, prediction direction
	dc_recon(block_num, &ld->block[0]);

	if (coded) 
	{
		const unsigned char * zigzag; // zigzag scan dir
		register int q_scale = (mp4_hdr.quantizer) << 1;
		register int q_add = ((q_scale>>1) & 1) ? (q_scale>>1) : ((q_scale>>1) - 1);
		
		if (mp4_hdr.ac_pred_flag == 1) {

			if (coeff_pred->predict_dir == TOP)
				zigzag = alternate_horizontal_scan;
			else
				zigzag = alternate_vertical_scan;
		}
		else {
			zigzag = zig_zag_scan;
		}
		
		i = 1;
		do // event vld
		{
			event = vld_intra_dct();
					
			i+= event.run;
			if (event.level > 0) {
				ld->block[zigzag[i]] = (q_scale * event.level) + q_add;
			}
			else {
				ld->block[zigzag[i]] = (q_scale * event.level) - q_add;
			}
			
			i++;
		} while (! event.last);
		
	}

	// ac reconstruction
	ac_recon(block_num, &ld->block[0]);

	return 1;
}

/**/

int blockInter(int block_num)
{
	event_t event;
	int i;
	register int q_scale = (mp4_hdr.quantizer) << 1;
	register int q_add = ((q_scale>>1) & 1) ? (q_scale>>1) : ((q_scale>>1) - 1);
	
		i = 0;
		do // event vld
		{
			event = vld_inter_dct();
			i+= event.run;
			if (event.level > 0) {
				ld->block[zig_zag_scan[i]] = (q_scale * event.level) + q_add;
			}
			else {
				ld->block[zig_zag_scan[i]] = (q_scale * event.level) - q_add;
			}
			i++;
		} while (! event.last);
			
	return 1;
}

/**/
#define _SWAPB(a) ((a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3]) 

int getDCsizeLum()
{
	int code;
	short n;
	unsigned char *v = ld->rdptr; 
	int rbit = 32 - ld->bitcnt; 
	unsigned int b,bt; 
	unsigned int array=-1; 

	b = _SWAPB(v); 
	bt=(b &(array>>(32-rbit)));
	for (n=11;n>3;n--)
	{
		if ((bt >> (rbit-n))==1)
		{
			flushbits(n);
		return n+1;
		}
}
	code = showbits(3);

	if (code == 1) {
		flushbits(3);
		return 4;
	} else if (code == 2) {
		flushbits(3);
		return 3;
	} else if (code == 3) {
		flushbits(3);
		return 0;
	}

  code = showbits(2);

  if (code == 2) {
		flushbits(2);
		return 2;
	} else if (code == 3) {
		flushbits(2);
		return 1;
	}     

	return 0;
}

int getDCsizeChr()
{
	// [Ag][note] bad code
	short n;
	unsigned char *v = ld->rdptr; 
	int rbit = 32 - ld->bitcnt; 
	unsigned int b,bt; 
	unsigned int array=-1; 

	b = _SWAPB(v); 
	bt=(b &(array>>(32-rbit)));
	for (n=12;n>2;n--)
	{
		if ((bt >> (rbit-n))==1)
		{
			flushbits(n);
			return n;
		}
	}
	return (3 - getbits(2));
}

/**/

int getDCdiff(int dct_dc_size)
{
	int code = getbits(dct_dc_size);
	int msb = code >> (dct_dc_size - 1);
	if (msb == 0) {
		return (-(code^((1<<dct_dc_size) - 1)));
	}
	else { 
		return code;
	}
}

/**/

void setDCscaler(int block_num) 
{
	int type = (block_num < 4) ? 0 : 1;
	int quant = mp4_hdr.quantizer;
	if (type == 0) {
		if (quant >24) mp4_hdr.dc_scaler = (2 * quant - 16);
		else if (quant>8) mp4_hdr.dc_scaler = (quant + 8);
		else if (quant>4) mp4_hdr.dc_scaler = (2 * quant);
		else mp4_hdr.dc_scaler = 8;
	}
	else {
		if (quant > 24) mp4_hdr.dc_scaler = (quant - 6);
		if (quant > 4) mp4_hdr.dc_scaler = ((quant + 13) / 2);
		else mp4_hdr.dc_scaler = 8;
	}
}

#ifdef _WIN32_WCE
#ifdef MIPS
	#ifndef MIPS32
	//it finally uses 64bits MIPSIII instructions!!
	void clearblock (idct_block_t *psblock) 
	{ 
	__asm ("addi $2,$0,2;" 
		"comienzo:sdr $0,0($4);" 
		"sdr $0,8($4);" 
		"sdr $0,16($4);" 
		"sdr $0,24($4);" 
		"sdr $0,32($4);" 
		"sdr $0,40($4);" 
		"sdr $0,48($4);" 
		"sdr $0,56($4);" 
		"addi $4,$4,64;" 
		"addi $2,$2,-1;" 
		"bgtz $2,comienzo;",psblock); 
	} 
	#else
	void clearblock (idct_block_t *psblock) 
	{ 
	__asm ("addi $2,$0,2;" 
		"comienzo:sw $0,0($4);" 
		"sw $0,4($4);" 
		"sw $0,8($4);" 
		"sw $0,12($4);" 
		"sw $0,16($4);" 
		"sw $0,20($4);" 
		"sw $0,24($4);" 
		"sw $0,28($4);" 
		"sw $0,32($4);" 
		"sw $0,36($4);" 
		"sw $0,40($4);" 
		"sw $0,44($4);" 
		"sw $0,48($4);" 
		"sw $0,52($4);" 
		"sw $0,56($4);" 
		"sw $0,60($4);" 
		"addi $4,$4,64;" 
		"addi $2,$2,-1;" 
		"bgtz $2,comienzo;",psblock); 
	} 
	#endif
#else

void clearblock (idct_block_t *psblock)
{
	int i;	

	for (i=64; i!=0;i--)
	{
		psblock++[0]=0;	
	}
}
#endif
#else

void clearblock (idct_block_t *psblock)
{
	int i;	

	for (i=64; i!=0;i--)
	{
		psblock++[0]=0;	
	}
}
#endif

/**/

int getMCBPC();
int getCBPY();
int setMV(int block_num);
int getMVdata();

/**/

void addblock (int comp, int bx, int by,int idct_mode)
{
	int cc;
	unsigned char *rfp;
	cc = (comp < 4) ? 0 : (comp & 1) + 1; /* color component index */

	if (cc == 0) // luminance
	{
		// pixel coordinates
		bx <<= 4;
		by <<= 4;
	    // frame DCT coding
		rfp = frame_ref[0] + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
		idct_special(ld->block, rfp, coded_picture_width,idct_mode);
		return;
	} 
	else // chrominance
	{
		// pixel coordinates
		bx <<= 3;
		by <<= 2;  //chrominance  
		// frame DCT coding
		rfp = frame_ref[cc] + coded_picture_width * by + bx;
		idct_special(ld->block, rfp, coded_picture_width>>1,idct_mode);
	}
}

/**/


// int x, y: block coord
// int block block num
// int mv comp (0: x, 1: y)
//
// Purpose: compute motion vector prediction

int find_pmv0 (int comp)
{
	int p1, p2, p3;
 	int x = mp4_hdr.mb_xpos;
	int y = mp4_hdr.mb_ypos;
	
	if (y == 0) 
	{
		if (x == 0) 
			return 0;
		else // block == 0
			return MV[x][y+1][1][comp];
	}
	else
	{
		// considerate border (avoid increment inside each single array index)
		x++;
		y++;
		
		p1 = MV[x-1][y][1][comp];
		p2 = MV[x][y-1][2][comp];
		p3 = MV[x+1][y-1][2][comp];

		return p1 + p2 + p3 - mmax (p1, mmax (p2, p3)) - mmin (p1, mmin (p2, p3));
	}
}

int find_pmv (int block, int comp)
{
	int p1, p2, p3;
	
	int x = mp4_hdr.mb_xpos;
	int y = mp4_hdr.mb_ypos;
	
	if ((y == 0) &&(block == 1))
	{
		return MV[x+1][y+1][0][comp];
	}
	else
	{
		// considerate border (avoid increment inside each single array index)
		x++;
		y++;

		switch (block)
		{
			case 1:
				p1 = MV[x][y][0][comp];
				p2 = MV[x][y-1][3][comp];
				p3 = MV[x+1][y-1][2][comp];
				break;
			case 2:
				p1 = MV[x-1][y][3][comp];
				p2 = MV[x][y][0][comp];
				p3 = MV[x][y][1][comp];
				break;
			default: // case 3
				p1 = MV[x][y][2][comp];
				p2 = MV[x][y][0][comp];
				p3 = MV[x][y][1][comp];
				break;
		}
	
		return p1 + p2 + p3 - mmax (p1, mmax (p2, p3)) - mmin (p1, mmin (p2, p3));
	}
}

/***/

int macroblock()
{
	int j;
	int interFlag=0;

	if (mp4_hdr.prediction_type != I_VOP)
		mp4_hdr.not_coded = getbits1();

	// coded macroblock or I-VOP
	if (! mp4_hdr.not_coded || mp4_hdr.prediction_type == I_VOP) {

		mp4_hdr.mcbpc = getMCBPC(); // mcbpc
		mp4_hdr.derived_mb_type = mp4_hdr.mcbpc & 7;
		mp4_hdr.cbpc = (mp4_hdr.mcbpc >> 4) & 3;

		modemap[mp4_hdr.mb_ypos+1][mp4_hdr.mb_xpos+1] = mp4_hdr.derived_mb_type; // [Review] used only in P-VOPs
		switch(mp4_hdr.derived_mb_type)
		{
			case INTRA:
				interFlag=0;
				mp4_hdr.ac_pred_flag = getbits1();
				mp4_hdr.cbpy = getCBPY(); // cbpy
				mp4_hdr.cbp = (mp4_hdr.cbpy << 2) | mp4_hdr.cbpc;
				if (mp4_hdr.prediction_type == P_VOP) {
					#ifdef MIPS
						__asm ("sdr		$0,0($4);"
							   "sdr		$0,8($4);",&MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][0]);
					#else 
						int i;
						for (i = 0; i < 4; i++) {
							MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][i][0] = 0;
							MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][i][1] = 0;
						}
					#endif
				}
			break;
			case INTRA_Q:
				interFlag=0;
				mp4_hdr.ac_pred_flag = getbits1();
				mp4_hdr.cbpy = getCBPY(); // cbpy
				mp4_hdr.cbp = (mp4_hdr.cbpy << 2) | mp4_hdr.cbpc;
				mp4_hdr.dquant = getbits(2);
				mp4_hdr.quantizer += DQtab[mp4_hdr.dquant];
				if (mp4_hdr.quantizer > 31)
					mp4_hdr.quantizer = 31;
				else if (mp4_hdr.quantizer < 1)
					mp4_hdr.quantizer = 1;
				if (mp4_hdr.prediction_type == P_VOP) {
					#ifdef MIPS
						__asm ("sdr		$0,0($4);"
							   "sdr		$0,8($4);",&MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][0]);
					#else 
						int i;
						for (i = 0; i < 4; i++) {
							MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][i][0] = 0;
							MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][i][1] = 0;
						}
					#endif
				}
			break;
			case INTER_Q:
				interFlag=1;
				mp4_hdr.cbpy = getCBPY(); // cbpy
				mp4_hdr.cbp = (mp4_hdr.cbpy << 2) | mp4_hdr.cbpc;
				mp4_hdr.dquant = getbits(2);
				mp4_hdr.quantizer += DQtab[mp4_hdr.dquant];
				if (mp4_hdr.quantizer > 31)
					mp4_hdr.quantizer = 31;
				else if (mp4_hdr.quantizer < 1)
					mp4_hdr.quantizer = 1;
				setMV(-1); // mv
			break;
			case INTER:
				interFlag=1;
				mp4_hdr.cbpy = getCBPY(); // cbpy
				mp4_hdr.cbp = (mp4_hdr.cbpy << 2) | mp4_hdr.cbpc;
				setMV(-1); // mv
				break;
			case INTER4V:
				interFlag=1;
				mp4_hdr.cbpy = getCBPY(); // cbpy
				mp4_hdr.cbp = (mp4_hdr.cbpy << 2) | mp4_hdr.cbpc;
				for (j = 0; j < 4; j++) {
					setMV(j); // mv
				}
			break;
			case STUFFING:
				return 1;
			break;
			}
		
		// motion compensation
		
		if (interFlag) 
		{
			reconstruct(mp4_hdr.mb_xpos, mp4_hdr.mb_ypos, mp4_hdr.derived_mb_type);
			// texture decoding add
			for (j = 0; j < 6; j++) {
				int coded = mp4_hdr.cbp & (1 << (5 - j));
				if (coded){
					blockInter(j);
					addblock(j, mp4_hdr.mb_xpos, mp4_hdr.mb_ypos,1);
				}
			}	
		}
		else 
		{
			// texture decoding add
			for (j = 0; j < 6; j++) {
				int coded = mp4_hdr.cbp & (1 << (5 - j));
				blockIntra(j, coded);
				addblock(j, mp4_hdr.mb_xpos, mp4_hdr.mb_ypos,0);
			}
		}
	}

	// not coded macroblock
	else {
		if (modemap[mp4_hdr.mb_ypos+1][mp4_hdr.mb_xpos+1] != NOT_CODED){

			modemap[mp4_hdr.mb_ypos+1][mp4_hdr.mb_xpos+1] = NOT_CODED; // [Review] used only in P-VOPs
			
			#ifdef MIPS

				#ifndef MIPS32
				__asm ("sdr		$0,0($4);"
					   "sdr		$0,8($4);",&MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][0]);
				#else 
				__asm ("sw		$0,0($4);"
					   "sw		$0,4($4);"
					   "sw		$0,8($4);"
					   "sw		$0,12($4);",&MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][0]);
				#endif
			#else 	
			
			MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][0] = MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][1] = 0;
			MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][1][0] = MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][1][1] = 0;
			MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][2][0] = MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][2][1] = 0;
			MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][3][0] = MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][3][1] = 0;
			
			#endif

			reconstruct(mp4_hdr.mb_xpos, mp4_hdr.mb_ypos, mp4_hdr.derived_mb_type);
		
		}
	}

	if (mp4_hdr.mb_xpos < (mp4_hdr.mb_xsize-1)) {
		mp4_hdr.mb_xpos++;
	}
	else {
		mp4_hdr.mb_ypos++;
		mp4_hdr.mb_xpos = 0;
	}
	return 1;
}

/**/

int getMCBPC()
{
	if (mp4_hdr.prediction_type == I_VOP)
	{
		int code = showbits(9);
		if (code == 1) {
			flushbits(9); // stuffing
			return 0;
		}
		else if (code < 8) {
			return -1;
		}
		code >>= 3;
		if (code >= 32) {
			flushbits(1);
			return 3;
		}
		flushbits(MCBPCtabIntra[(code<<1)+1]);
		return MCBPCtabIntra[code<<1];
	}
	else
	{
		int code = showbits(9);

		if (code == 1) {
			flushbits(9); // stuffing
			return 0;
		}
		else if (code == 0)	{
			return -1;
		}
		
		if (code >= 256)
		{
			flushbits(1);
			return 0;
		}
		
		flushbits(MCBPCtabInter[(code<<1)+1]);
		return MCBPCtabInter[code<<1];
	}
}

/**/

int getCBPY()
{
	int cbpy;
	int code = showbits(6);
	if (code < 2) {
		return -1;
	}
	if (code >= 48) {
		flushbits(2);
		cbpy = 15;
	} else {
		flushbits(CBPYtab[(code<<1)+1]);
		cbpy = CBPYtab[code<<1];
	}
	if (!((mp4_hdr.derived_mb_type == 3) ||
		(mp4_hdr.derived_mb_type == 4)))
		  cbpy = 15 - cbpy;
	return cbpy;
}

/**/

int setMV(int block_num)
{
	int hor_mv_data, ver_mv_data, hor_mv_res, ver_mv_res;
	int scale_fac = 1 << (mp4_hdr.fcode_for - 1);
	int high = (/*32 **/ scale_fac<<5) - 1;
	int low = -(/*32 **/ scale_fac<<5);
	int range = (/*64 **/ scale_fac<<6);

	int mvd_x, mvd_y, pmv_x, pmv_y, mv_x, mv_y;
	
	hor_mv_data = getMVdata(); // mv data

	if ((scale_fac == 1) || (hor_mv_data == 0))
		mvd_x = hor_mv_data;
	else {
		hor_mv_res = getbits(mp4_hdr.fcode_for-1); // mv residual
		mvd_x = ((abs(hor_mv_data) - 1) * scale_fac) + hor_mv_res + 1;
		if (hor_mv_data < 0)
			mvd_x = - mvd_x;
	}
	
	ver_mv_data = getMVdata(); 
	
	if ((scale_fac == 1) || (ver_mv_data == 0))
		mvd_y = ver_mv_data;
	else {
		ver_mv_res = getbits(mp4_hdr.fcode_for-1);
		mvd_y = ((abs(ver_mv_data) - 1) * scale_fac) + ver_mv_res + 1;
		if (ver_mv_data < 0)
			mvd_y = - mvd_y;
	}
	
	if (block_num >0) {
		pmv_x = find_pmv(block_num, 0);
		pmv_y = find_pmv(block_num, 1);
	}
	
	else {
		pmv_x = find_pmv0(0);
		pmv_y = find_pmv0(1);
	}
	
	mv_x = pmv_x + mvd_x;
	
	if (mv_x < low)
		mv_x += range;
	
	if (mv_x > high)
		mv_x -= range;
		
	mv_y = pmv_y + mvd_y;
	
	if (mv_y < low)
		mv_y += range;
	
	if (mv_y > high)
		mv_y -= range;
	
	// put [mv_x, mv_y] in MV struct
	if (block_num == -1) {
		#ifdef MIPS
			__asm ("sll		$6,$6,16;"
				   "andi	$5,$5,0xFFFF;"
				   "or		$8,$6,$5;"
				   "sw		$8,0($4);"
				   "sw		$8,4($4);"
				   "sw		$8,8($4);"
				   "sw		$8,12($4);",&MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][0][0],mv_x,mv_y);
		#else 
		int i;
		for (i = 0; i < 4; i++) {
			MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][i][0] = mv_x;
			MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][i][1] = mv_y;
		}
		#endif
	}
	else {
		MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][block_num][0] = mv_x;
		MV[mp4_hdr.mb_xpos+1][mp4_hdr.mb_ypos+1][block_num][1]= mv_y;
	}
	return 1;
}

/**/

int getMVdata()
{
	int code;

	if (getbits1()) {
		return 0; // hor_mv_data == 0
	}
	
	code = showbits(12);

	if (code >= 512)
	{
		code = ((code >> 8) - 2)<<1;
		flushbits(MVtab0[code+1]);
		return MVtab0[code];
	}
	
	if (code >= 128)
	{
		code = ((code >> 2) - 32)<<1;
		flushbits(MVtab1[code+1]);
		return MVtab1[code];
	}

	code =(code-4)<<1; 

	flushbits(MVtab2[code+1]);
	return MVtab2[code];
}

/**/

#endif
