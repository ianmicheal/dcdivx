 /*This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The GPL can be found at: http://www.gnu.org/copyleft/gpl.html						*
 *																						*
 **************************************************************************
 *  Copyright (C) 2001 - Project Mayo                                     *
 *																		  *		
 * Andrea Graziani (Ag)													  *			
 *                                                                        *      
 * DivX Advanced Research Center <darc@projectmayo.com>					  *
 *                                                                        *    
 **************************************************************************/

// mp4_picture.c //

// INCLUDE
#include "kos.h"
#include "mp4_decoder.h"
#include "global.h"
#include "yuv2rgb.h"
#include "decore.h"
// FUNCTION DECLARATIONS

extern int macroblock();

void get_mp4picture (unsigned char *bmp, int render_flag,int Extra);
void make_edge (unsigned char *frame_pic, int width, int height, int edge);
void PictureDisplay(unsigned char *bmp, int render_flag,int Extra);

// GLOBAL VAR DECLARATIONS

int Dither;

// FUNCTION CODE

// Purpose: decode and display a Vop

void get_mp4picture (unsigned char *bmp, int render_flag,int Extra)
{
	mp4_hdr.mba = 0;
	mp4_hdr.mb_xpos = 0;
	mp4_hdr.mb_ypos = 0;

	if (!mp4_hdr.prediction_type == I_VOP){

		make_edge (frame_for[0], coded_picture_width, coded_picture_height, 32);
		make_edge (frame_for[1], coded_picture_width>>1, coded_picture_height>>1, 16);
		make_edge (frame_for[2], coded_picture_width>>1, coded_picture_height>>1, 16);

	}
	do {
		macroblock();
		mp4_hdr.mba++; 
	} while ((nextbits_bytealigned(23) != 0) && (mp4_hdr.mba < mp4_hdr.mba_size));

	// add edge to decoded frame

	PictureDisplay(bmp, render_flag,Extra);

	// exchange ref and for frames
	{
		int i;
		unsigned char *tmp;
		for (i = 0; i < 3; i++) {
			tmp = frame_ref[i];
			frame_ref[i] = frame_for[i];
			frame_for[i] = tmp;
		}
	}
}


/**/

void make_edge (unsigned char *frame_pic,
                int edged_width, int edged_height, int edge)
{
	int j;

	int width = edged_width - (edge<<1);
	int height = edged_height - (edge<<1);
	
	unsigned char *p_border;
	unsigned char *p_border_top, *p_border_bottom;
	unsigned char *p_border_top_ref, *p_border_bottom_ref;

	// left and right edges
	
	p_border = frame_pic;

	for (j = height;j!=0 ;j--)
	{

		memset((p_border - edge), *(p_border), edge);
		memset((p_border + width),*(p_border + (width-1)), edge);

		p_border += edged_width;
	}

	// top and bottom edges
	
	p_border_top_ref = frame_pic;
	p_border_bottom_ref = frame_pic + (edged_width * (height -1));
	p_border_top = p_border_top_ref - (edge * edged_width);
	p_border_bottom = p_border_bottom_ref + edged_width;

	// corners
	
	{
		unsigned char * p_left_corner_top = frame_pic - edge - (edge * edged_width);
		unsigned char * p_right_corner_top = p_left_corner_top + edge + width;
		unsigned char * p_left_corner_bottom = frame_pic + (edged_width * height) - edge;
		unsigned char * p_right_corner_bottom = p_left_corner_bottom + edge + width;

		char left_corner_top = *(frame_pic);
		char right_corner_top = *(frame_pic + (width-1));
		int ethm1 = (edged_width * (height-1));
		char left_corner_bottom = *(frame_pic + ethm1);
		char right_corner_bottom = *(frame_pic + ethm1 + (width-1));

		for (j = edge;j!=0; j--)
		{
			memset(p_left_corner_top, left_corner_top, edge);
			memset(p_right_corner_top, right_corner_top, edge);
			memset(p_left_corner_bottom, left_corner_bottom, edge);
			memset(p_right_corner_bottom, right_corner_bottom, edge);

			p_left_corner_top += edged_width;
			p_right_corner_top += edged_width;
			p_left_corner_bottom += edged_width;
			p_right_corner_bottom += edged_width;

			memcpy(p_border_top, p_border_top_ref, width);
			memcpy(p_border_bottom, p_border_bottom_ref, width);

			p_border_top += edged_width;
			p_border_bottom += edged_width;
		}
	}
}
/**/

// Purpose: manages a one frame buffer for re-ordering frames prior to 
// displaying or writing to a file

void PictureDisplay(unsigned char *bmp, int render_flag,int Extra)
{ 
	AVPicture * p=(AVPicture *) bmp;
/*	
		for(i=0; i<mp4_hdr.height; i++) {
		    
		    memcpy(p->data[0]+ i*mp4_hdr.width,
			   frame_ref[0] + i*coded_picture_width,
			   mp4_hdr.width);
		}
		
		for(i=0; i<mp4_hdr.height/2; i++) {
		    
		    memcpy(p->data[1] + i*mp4_hdr.width/2,
			   frame_ref[2] + i*coded_picture_width/2,
			   mp4_hdr.width/2);
		    
		    memcpy(p->data[2] + i*mp4_hdr.width/2,
			   frame_ref[1] + i*coded_picture_width/2,
			   mp4_hdr.width/2);
		}
*/	
	p->data[0]=frame_ref[0];
	p->data[1]=frame_ref[1];
	p->data[2]=frame_ref[2];
	p->linesize[0]=coded_picture_width;
	p->linesize[1]=coded_picture_width>>1;
	p->linesize[2]=coded_picture_width>>1;
#ifdef ARM
	p->last_picture[0]=frame_for[0];
	p->last_picture[1]=frame_for[1];
	p->last_picture[2]=frame_for[2];
#endif
}
