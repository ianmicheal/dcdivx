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
	
// mp4_header.h //

#ifndef _MP4_HEADER_H_
#define _MP4_HEADER_H_

#define VO_START_CODE					0x8
#define VOL_START_CODE					0x12
#define VOP_START_CODE					0x1b6

#define I_VOP							0
#define P_VOP							1
#define B_VOP							2

#define RECTANGULAR						0
#define BINARY							1
#define BINARY_SHAPE_ONLY				2 
#define GREY_SCALE						3

#define STATIC_SPRITE					1

#define USER_DATA_START_CODE			0x01b2

#define NOT_CODED						-1
#define INTER							0
#define INTER_Q							1
#define INTER4V							2
#define INTRA							3	
#define INTRA_Q	 						4
#define STUFFING						7

/**/

typedef struct _mp4_header {

	// vol
	
	int is_object_layer_identifier;
	int visual_object_layer_verid;
	int visual_object_layer_priority;
	int shape;
	int time_increment_resolution;
	int width;
	int height;
	int quant_precision;
	int bits_per_pixel;
	int scalability;

	// vop

	int prediction_type;
	int time_base;
	int time_inc;
	int vop_coded;
	int rounding_type;
	int quantizer;
	int fcode_for;
	
	// macroblock

	int not_coded;
	int mcbpc;
	int derived_mb_type;
	int cbpc;
	int ac_pred_flag;
	int cbpy;
	int dquant;
	int cbp;

	// extra/derived

	int mba_size;
	int mb_xsize;
	int mb_ysize;
	int picnum;
	int mba;
	int mb_xpos;
	int mb_ypos;
	int dc_scaler;

} mp4_header;

/**/

extern int getvolhdr();
extern int getvophdr();
extern int bytealigned(int nbits);
extern void next_start_code();
extern int nextbits_bytealigned(int nbit);

#endif // _MP4_HEADER_H_

