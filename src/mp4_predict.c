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

// mp4_predict.c //

#include "mp4_decoder.h"
#include "global.h"
#include "mp4_predict.h"

/**/

void dc_recon(int block_num, idct_block_t * dc_value);
void ac_recon(int block_num, idct_block_t * psBlock);
void rescue_predict();

/*

	B - C
	|   |
	A - x

*/

void dc_recon(int block_num, idct_block_t * dc_value)
{
	if (mp4_hdr.prediction_type == P_VOP) {
		rescue_predict();
	}

	if (block_num < 4)
	{
		int b_xpos = (mp4_hdr.mb_xpos << 1) + (block_num & 1);
		int b_ypos = (mp4_hdr.mb_ypos << 1) + ((block_num & 2) >> 1);
		int dc_pred;

		// set prediction direction
		if (abs(coeff_pred->ac_dc_left_lum[b_ypos][b_xpos][0] -
			coeff_pred->ac_dc_left_lum[b_ypos+1][b_xpos][0]) < // Fa - Fb
			abs(coeff_pred->ac_dc_left_lum[b_ypos][b_xpos][0] -
			coeff_pred->ac_dc_left_lum[b_ypos][b_xpos+1][0])) // Fb - Fc
			{
				coeff_pred->predict_dir = TOP;
				dc_pred = coeff_pred->ac_dc_left_lum[b_ypos][b_xpos+1][0];
			}
		else
			{
				coeff_pred->predict_dir = LEFT;
				dc_pred = coeff_pred->ac_dc_left_lum[b_ypos+1][b_xpos][0];
			}

		(* dc_value) += _div_div(dc_pred, mp4_hdr.dc_scaler);
		(* dc_value) *= mp4_hdr.dc_scaler;

		// store dc value
		coeff_pred->ac_dc_left_lum[b_ypos+1][b_xpos+1][0] = (* dc_value);
	}
	else // chrominance blocks
	{
		int b_xpos = mp4_hdr.mb_xpos;
		int b_ypos = mp4_hdr.mb_ypos;
		int chr_num = block_num - 4;
		int dc_pred;

		// set prediction direction
		if (abs(coeff_pred->ac_dc_left_chr[chr_num][b_ypos][b_xpos][0] -
			coeff_pred->ac_dc_left_chr[chr_num][b_ypos+1][b_xpos][0]) < // Fa - Fb
			abs(coeff_pred->ac_dc_left_chr[chr_num][b_ypos][b_xpos][0] -
			coeff_pred->ac_dc_left_chr[chr_num][b_ypos][b_xpos+1][0])) // Fb - Fc
			{
				coeff_pred->predict_dir = TOP;
				dc_pred = coeff_pred->ac_dc_left_chr[chr_num][b_ypos][b_xpos+1][0];
			}
		else
			{
				coeff_pred->predict_dir = LEFT;
				dc_pred = coeff_pred->ac_dc_left_chr[chr_num][b_ypos+1][b_xpos][0];
			}

		(* dc_value) += _div_div(dc_pred, mp4_hdr.dc_scaler);
		(* dc_value) *= mp4_hdr.dc_scaler;

		// store dc value
		coeff_pred->ac_dc_left_chr[chr_num][b_ypos+1][b_xpos+1][0] = (* dc_value);
	}
}

/**/

void ac_recon(int block_num, idct_block_t * psBlock)
{
	int b_xpos, b_ypos;
	int i;

	if (block_num < 4) {
		b_xpos = (mp4_hdr.mb_xpos << 1) + (block_num & 1);
		b_ypos = (mp4_hdr.mb_ypos << 1) + ((block_num & 2) >> 1);
	}
	else {
		b_xpos = mp4_hdr.mb_xpos;
		b_ypos = mp4_hdr.mb_ypos;
	}

	// predict coefficients
	if (mp4_hdr.ac_pred_flag) 
	{
		if (block_num < 4) 
		{
			if (coeff_pred->predict_dir == TOP)
			{
				for (i = 1; i < 8; i++) // [Review] index can become more efficient [0..7]
					psBlock[i] += coeff_pred->ac_dc_top_lum[b_ypos+1-1][b_xpos+1][i];
			}
			else // left prediction
			{
				for (i = 1; i < 8; i++)
					psBlock[i<<3] += coeff_pred->ac_dc_left_lum[b_ypos+1][b_xpos][i];
			}
		}
		else
		{
			int chr_num = block_num - 4;

			if (coeff_pred->predict_dir == TOP)
			{
				for (i = 1; i < 8; i++)
					psBlock[i] += coeff_pred->ac_dc_top_chr[chr_num][b_ypos+1-1][b_xpos+1][i];
			}
			else // left prediction
			{
				for (i = 1; i < 8; i++)
					psBlock[i<<3] += coeff_pred->ac_dc_left_chr[chr_num][b_ypos+1][b_xpos][i];
			}
		}
	}

	// store coefficients
	if (block_num < 4)
	{
		for (i = 1; i < 8; i++) {
			coeff_pred->ac_dc_top_lum[b_ypos+1][b_xpos+1][i] = psBlock[i];
			coeff_pred->ac_dc_left_lum[b_ypos+1][b_xpos+1][i] = psBlock[i<<3];
		}
	}
	else 
	{
		int chr_num = block_num - 4;

		for (i = 1; i < 8; i++) {
			coeff_pred->ac_dc_top_chr[chr_num][b_ypos+1][b_xpos+1][i] = psBlock[i];
			coeff_pred->ac_dc_left_chr[chr_num][b_ypos+1][b_xpos+1][i] = psBlock[i<<3];
		}
	}
}

/**/

#define _IsIntra(mb_y, mb_x) ((modemap[(mb_y)+1][(mb_x)+1] == INTRA) || \
	(modemap[(mb_y)+1][(mb_x)+1] == INTRA_Q))

void rescue_predict() 
{
	int mb_xpos = mp4_hdr.mb_xpos;
	int mb_ypos = mp4_hdr.mb_ypos;
	int i;

	if (! _IsIntra(mb_ypos-1, mb_xpos-1)) {
		// rescue -A- DC value
		coeff_pred->ac_dc_left_lum[2*mb_ypos][2*mb_xpos][0] = 1024;
		coeff_pred->ac_dc_left_chr[0][mb_ypos][mb_xpos][0] = 1024;
		coeff_pred->ac_dc_left_chr[1][mb_ypos][mb_xpos][0] = 1024;
	}
	// left
	if (! _IsIntra(mb_ypos, mb_xpos-1)) {
		#ifdef MIPS
			#ifndef MIPS32
			__asm("addi		$8,$0,1024;"
				  "sw		$8,0($4);"
				  "sw		$0,4($4);"
				  "sdr		$0,8($4);"
				  "sw		$8,0($5);"
				  "sw		$0,4($5);"
				  "sdr		$0,8($5);"
				  "sw		$8,0($6);"
				  "sw		$0,4($6);"
				  "sdr		$0,8($6);"
				  "sw		$8,0($7);"
				  "sw		$0,4($7);"
				  "sdr		$0,8($7);",&coeff_pred->ac_dc_left_lum[2*mb_ypos+1][2*mb_xpos][0],&coeff_pred->ac_dc_left_lum[2*mb_ypos+2][2*mb_xpos][0],&coeff_pred->ac_dc_left_chr[0][mb_ypos+1][mb_xpos][0],&coeff_pred->ac_dc_left_chr[1][mb_ypos+1][mb_xpos][0]);
			#else
				__asm("addi		$8,$0,1024;"
				  "sw		$8,0($4);"
				  "sw		$0,4($4);"
				  "sw		$0,8($4);"
				  "sw		$0,12($4);"
				  "sw		$8,0($5);"
				  "sw		$0,4($5);"
				  "sw		$0,8($5);"
				  "sw		$0,12($5);"
				  "sw		$8,0($6);"
				  "sw		$0,4($6);"
				  "sw		$0,8($6);"
				  "sw		$0,12($6);"
				  "sw		$8,0($7);"
				  "sw		$0,4($7);"
				  "sw		$0,8($7);"
				  "sw		$0,12($7);",&coeff_pred->ac_dc_left_lum[2*mb_ypos+1][2*mb_xpos][0],&coeff_pred->ac_dc_left_lum[2*mb_ypos+2][2*mb_xpos][0],&coeff_pred->ac_dc_left_chr[0][mb_ypos+1][mb_xpos][0],&coeff_pred->ac_dc_left_chr[1][mb_ypos+1][mb_xpos][0]);
			#endif
		#else
			// rescue -B- DC values
			coeff_pred->ac_dc_left_lum[2*mb_ypos+1][2*mb_xpos][0] = 1024;
			coeff_pred->ac_dc_left_lum[2*mb_ypos+2][2*mb_xpos][0] = 1024;
			coeff_pred->ac_dc_left_chr[0][mb_ypos+1][mb_xpos][0] = 1024;
			coeff_pred->ac_dc_left_chr[1][mb_ypos+1][mb_xpos][0] = 1024;
			//  rescue -B- AC values
			for(i = 1; i < 8; i++) {
				coeff_pred->ac_dc_left_lum[2*mb_ypos+1][2*mb_xpos][i] = 0;
				coeff_pred->ac_dc_left_lum[2*mb_ypos+2][2*mb_xpos][i] = 0;
				coeff_pred->ac_dc_left_chr[0][mb_ypos+1][mb_xpos][i] = 0;
				coeff_pred->ac_dc_left_chr[1][mb_ypos+1][mb_xpos][i] = 0;
			}
		#endif
	}
	// top
	if (! _IsIntra(mb_ypos-1, mb_xpos)) {
		// rescue -C- DC values
		coeff_pred->ac_dc_left_lum[2*mb_ypos][2*mb_xpos+1][0] = 1024;
		coeff_pred->ac_dc_left_lum[2*mb_ypos][2*mb_xpos+2][0] = 1024;
		coeff_pred->ac_dc_left_chr[0][mb_ypos][mb_xpos+1][0] = 1024;
		coeff_pred->ac_dc_left_chr[1][mb_ypos][mb_xpos+1][0] = 1024;
		// rescue -C- AC values
		for(i = 1; i < 8; i++) {
			coeff_pred->ac_dc_top_lum[2*mb_ypos][2*mb_xpos+1][i] = 0;
			coeff_pred->ac_dc_top_lum[2*mb_ypos][2*mb_xpos+2][i] = 0;
			coeff_pred->ac_dc_top_chr[0][mb_ypos][mb_xpos+1][i] = 0;
			coeff_pred->ac_dc_top_chr[1][mb_ypos][mb_xpos+1][i] = 0;
		}
	}
}
