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
 *                                                                        *        
 **************************************************************************/

// mp4_header.c //
#include "kos.h"
#include "mp4_decoder.h"
#include "global.h"
#include "mp4_header.h"

void next_start_code();

/**/

int getvolhdr()
{
	if (showbits(27) == VO_START_CODE)
	{
		int code;
		flushbits(73); // start_code + vo_id + vol_id +...
		
		mp4_hdr.is_object_layer_identifier = getbits1();
		if (mp4_hdr.is_object_layer_identifier) {
			mp4_hdr.visual_object_layer_verid = getbits(4);
			mp4_hdr.visual_object_layer_priority = getbits(3);
		} 
		else {
			mp4_hdr.visual_object_layer_verid = 1;
			mp4_hdr.visual_object_layer_priority = 1;
		}
		flushbits(5);
		mp4_hdr.shape = getbits(2);
		flushbits(1); // marker
		mp4_hdr.time_increment_resolution = getbits(16);
		flushbits(1); // marker
		if (getbits1()) { //fixed_vop_rate
			int bits = (int) ceil(log((double)mp4_hdr.time_increment_resolution)/log(2.0));
			if (bits < 1) bits = 1;
			flushbits(bits);
		}

		flushbits(1); // marker
		mp4_hdr.width = getbits(13);
		flushbits(1); // marker
		mp4_hdr.height = getbits(13);
		flushbits(3); // marker + interlaced + obmc_disable
		if (mp4_hdr.visual_object_layer_verid == 1) {
			flushbits(1);
		} 
		else {
			flushbits(2);
		}
			
		code = getbits1();
		if (code == 1) 
		{
			mp4_hdr.quant_precision = getbits(4);
			mp4_hdr.bits_per_pixel = getbits(4);
		}
		else 
		{
			mp4_hdr.quant_precision = 5;
			mp4_hdr.bits_per_pixel = 8;
		}

		flushbits(1); // quant type
		//code=getbits(1);
		if (mp4_hdr.visual_object_layer_verid/*ident*/ != 1) {
			flushbits(1);
		} 

		flushbits(4);
//		code=getbits(1);
//		if (mp4_hdr.visual_object_layer_verid/*ident*/ != 1) {
//			code=getbits(1);
//			//flushbits(3);
//			code=getbits(1);
//		} 

		//mp4_hdr.scalability = getbits1();

//		if (showbits(32) == USER_DATA_START_CODE) {
//			exit(104);
//		}
  	return 1;
  }
  
  return 0; // no VO start code
}

/**/

int getvophdr()
{
	//int code=0;
	next_start_code();
	while (showbits(32) != (int) VOP_START_CODE)
	{
		flushbits(8);
		next_start_code();
//		code++;
		//return 0;
	}
	flushbits(32);
	mp4_hdr.prediction_type = getbits(2);
	if (mp4_hdr.prediction_type == B_VOP)
	{	
		//mp4_hdr.last_prediction_type=mp4_hdr.prediction_type;
		return 0;
	}
	while (getbits1() == 1); // temporal time base
//	{
//		mp4_hdr.time_base++;
//	}
	flushbits(1); // marker bit
	{
		int bits = (int) ceil(log(mp4_hdr.time_increment_resolution)/log(2.0));
		if (bits < 1) bits = 1;
		flushbits(bits+1);
	}
	//mp4_hdr.time_inc = getbits(bits); // vop_time_increment (1-16 bits)
	
//	flushbits(1); // marker bit
	mp4_hdr.vop_coded = getbits1();
	if (mp4_hdr.vop_coded == 0) 
	{
		//next_start_code();
		return 0;
	}  

	if (mp4_hdr.prediction_type == P_VOP)
	{
		mp4_hdr.rounding_type = getbits1();
	} else {
		mp4_hdr.rounding_type = 0;
	}

	flushbits(3);// intra_dc_vlc_thr
	mp4_hdr.quantizer = getbits(mp4_hdr.quant_precision); // vop quant

	if (mp4_hdr.prediction_type != I_VOP) 
	{
		mp4_hdr.fcode_for = getbits(3); 
	}
	
//	if (! mp4_hdr.scalability) {
//		if (mp4_hdr.shape && mp4_hdr.prediction_type!=I_VOP)
//			flushbits(1); // vop shape coding type
			/* motion_shape_texture() */
//	}
	
	return 1;
}
// Purpose: look nbit forward for an alignement
int bytealigned(int nbit) 
{
	return (((ld->bitcnt + nbit) << 29) == 0);
}

/**/

void next_start_code()
{
	if (juice_flag)
	{
		// juice_flag = 0; // [Ag] before juice needed this changed only first time
		if (! bytealigned(0))
		{
			getbits1();
			// bytealign
			while (! bytealigned(0)) {
				flushbits(1);
			}
		}
	}
	else
	{
		getbits1();
		// bytealign
		while (! bytealigned(0)) {
			flushbits(1);
		}
	}
}

/**/

int nextbits_bytealigned(int nbit)
{
	int code;
	int skipcnt = 0;
	if (bytealigned(skipcnt))
	{
		// stuffing bits
		if (showbits(8) == 127) {
			skipcnt += 8;
		}
	}
	else
	{
		// bytealign
		while (! bytealigned(skipcnt)) {
			skipcnt += 1;
		}
	}
	code = showbits(nbit + skipcnt);
	return ((code << skipcnt) >> skipcnt);
}
