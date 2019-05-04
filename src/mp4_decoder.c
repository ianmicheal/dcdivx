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
 **************************************************************************/

//INCLUDE

#define GLOBAL
#include "kos.h"
#include "mp4_decoder.h"
#include "global.h"

//FUNCTION DECLARATIONS

void initdecoder ();
void closedecoder ();

//FUNCTION CODE

void initdecoder ()
{
	int i, j, size_lum,size_crom;
	char * BigBuff;
	if (!tempBB)
	{
		BigBuff=tempBB=(char*)malloc(600 * 600* 3);// lum_width*lum_height*2 + chrom_width*chrom_height*4 (=lum_width*lum_height)
	//		BigBuff=tempBB=(char*)ta_txr_map(ta_txr_allocate(600 * 600* 3));// lum_width*lum_height*2 + chrom_width*chrom_height*4 (=lum_width*lum_height)
	}
	else
	{
		BigBuff=tempBB;
	}

//	char * BigBuff=tempBB=(char*)malloc(coded_picture_width * coded_picture_height*3);// lum_width*lum_height*2 + chrom_width*chrom_height*4 (=lum_width*lum_height)
	// dc prediction border 
	memset(BigBuff,0,coded_picture_width * coded_picture_height*5);
	for (i = 0; i < 64/*(2*MBC+1)*/; i++)
		coeff_pred->ac_dc_left_lum[0][i][0] = 1024;
	for (i = 1; i < 64/*(2*MBR+1)*/; i++)
		coeff_pred->ac_dc_left_lum[i][0][0] = 1024;
	for (i = 0; i < 32/*(MBC+1)*/; i++) {
		coeff_pred->ac_dc_left_chr[0][0][i][0] = 1024;
		coeff_pred->ac_dc_left_chr[1][0][i][0] = 1024;
	}
	for (i = 1; i < 32/*(MBR+1)*/; i++) {
		coeff_pred->ac_dc_left_chr[0][i][0][0] = 1024;
		coeff_pred->ac_dc_left_chr[1][i][0][0] = 1024;
	}

	// ac prediction border 
	
	for (i = 0; i < 64/*(2*MBC+1)*/; i++)
		for (j = 1; j < 8; j++)	{
			coeff_pred->ac_dc_left_lum[0][i][j] = 0;
			coeff_pred->ac_dc_top_lum[0][i][j] = 0;
		}
	for (i = 1; i < 64/*(2*MBR+1)*/; i++)
		for (j = 1; j < 8; j++)	{
			coeff_pred->ac_dc_left_lum[i][0][j] = 0;
			coeff_pred->ac_dc_top_lum[i][0][j] = 0;
		}
	
	for (i = 0; i < 32/*(MBC+1)*/; i++)
		for (j = 1; j < 8; j++)	{
			coeff_pred->ac_dc_left_chr[0][0][i][j] = 0; 
			coeff_pred->ac_dc_top_chr[0][0][i][j] = 0;
			coeff_pred->ac_dc_left_chr[1][0][i][j] = 0;
			coeff_pred->ac_dc_top_chr[1][0][i][j] = 0;
		}
	for (i = 1; i < 32/*(MBR+1)*/; i++)
		for (j = 1; j < 8; j++)	{
			coeff_pred->ac_dc_left_chr[0][i][0][j] = 0;
			coeff_pred->ac_dc_top_chr[0][i][0][j] = 0;
			coeff_pred->ac_dc_left_chr[1][i][0][j] = 0;
			coeff_pred->ac_dc_top_chr[1][i][0][j] = 0;
		}
	
	// mode border 

	for (i = 0; i < mp4_hdr.mb_xsize + 1; i++)
		modemap[0][i] = INTRA;
	for (i = 0; i < mp4_hdr.mb_ysize + 1; i++) {
		modemap[i][0] = INTRA;
		modemap[i][mp4_hdr.mb_xsize+1] = INTRA;
	}
	
	// edged forward and reference frame


	
	size_lum = coded_picture_width * coded_picture_height;
	size_crom = coded_picture_width * coded_picture_height>>2;
	frame_ref[0] = (unsigned char *) (BigBuff + (coded_picture_width<<5) + 32);
	BigBuff+= size_lum;
	frame_ref[1] = (unsigned char *) (BigBuff + (coded_picture_width<<3) + 16);
	BigBuff+= size_crom;
	frame_ref[2] = (unsigned char *) (BigBuff + (coded_picture_width<<3) + 16);
	BigBuff+= size_crom;

	frame_for[0] = (unsigned char *) (BigBuff + (coded_picture_width<<5) + 32);
	BigBuff+= size_lum;
	frame_for[1] = (unsigned char *) (BigBuff + (coded_picture_width<<3) + 16);
	BigBuff+= size_crom;
	frame_for[2] = (unsigned char *) (BigBuff + (coded_picture_width<<3) + 16);

}

/**/

void closedecoder ()
{
//	free(tempBB);
}
