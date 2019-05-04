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
 * Adam Li                                                                *
 *                                                                        *
 * DivX Advanced Research Center <darc@projectmayo.com>                   * 
 *                                                                        *   
 **************************************************************************/

// decore.c //

#include "mp4_decoder.h"
#include "global.h"
//#include "yuv2rgb.h"
#include "../decore.h"
#include "malloc.h"

/**/

extern unsigned char *decore_stream;
extern int Dither;
int decore_init(int hor_size, int ver_size, unsigned long color_depth, int output_format);
int decore_release();
int decore_frame(unsigned char *stream, int length, unsigned char *bmp, int render_flag, int Extra);
int decore_setoutput(unsigned long color_depth, int output_format);

/**/

static int flag_firstpicture = 1;

/**/

int decore(unsigned long handle, unsigned long dec_opt,
	void *param1, void *param2)
{
	switch (dec_opt)
	{
		case DEC_OPT_INIT:
			{DEC_PARAM *dec_param = (DEC_PARAM *) param1;
 			int x_size = dec_param->x_dim;
 			int y_size = dec_param->y_dim;
			unsigned long color_depth = dec_param->color_depth;
			int output_format = dec_param->output_format;
		
			juice_flag = 1;
			Dither=dec_param->dither;
			decore_init(x_size, y_size, color_depth, output_format); // init decoder resources

			return DEC_OK;
			}
			break; 
		case DEC_OPT_RELEASE:
			decore_release();
			return DEC_OK;
			break;
		default:
			return DEC_OK;
			break;
	}
}

/**/

int decore_init(int hor_size, int ver_size, unsigned long color_depth, int output_format)
{
	// init global stuff
	ld = &base;
	coeff_pred = &ac_dc;
	initbits ();
	// read first vol and vop
	mp4_hdr.width = hor_size;
	mp4_hdr.height = ver_size;
	mp4_hdr.quant_precision = 5;
	//mp4_hdr.bits_per_pixel = 8;
	flag_firstpicture=1;
	if (flag_firstpicture == 1) {
		mp4_hdr.time_increment_resolution = 0;
		flag_firstpicture = 0;
	}
//	mp4_hdr.picnum = 0;
    mp4_hdr.mb_xsize = ( mp4_hdr.width + 15 ) / 16; 
	mp4_hdr.mb_ysize = ( mp4_hdr.height + 15 ) / 16;
	mp4_hdr.mba_size = mp4_hdr.mb_xsize * mp4_hdr.mb_ysize;

	// set picture dimension global vars
	
	coded_picture_width = ((mp4_hdr.width + 64)+7)&0xFFF8;
	coded_picture_height = ((mp4_hdr.height + 64)+7)&0xFFF8;
	
	// init decoder

	initdecoder();
	//decore_setoutput(color_depth, output_format);
	return 1;
}

/**/

int decore_frame(unsigned char *stream, int length, unsigned char *bmp, int render_flag, int Extra)
{
	decore_stream = stream;
	initbits ();
	getvolhdr();
	if (!getvophdr()) return 0;; // read vop header
	get_mp4picture (bmp, render_flag,Extra); // decode vop
//	mp4_hdr.picnum++;
	return 1;
}

/**/

int decore_release()
{
	printf("closing decoder\r\n");
	closedecoder();
	printf("closed decoder\r\n");
	return 1;
}

/**/

int decore_setoutput(unsigned long color_depth, int output_format)
{
	return 1;
}
