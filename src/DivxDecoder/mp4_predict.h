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

// mp4_predict.h //

#ifndef _MP4_PREDICT_H_
#define _MP4_PREDICT_H_

#define TOP 1
#define LEFT 0

#endif // _MP4_PREDICT_H_

// aritmetic operators 

#define _div_div(a, b) (a>0) ? (a+(b>>1))/b : (a-(b>>1))/b

/***/

extern void dc_recon(int block_num, idct_block_t * dc_value);
extern void ac_recon(int block_num, idct_block_t * psBlock);
