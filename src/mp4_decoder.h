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
 *																	      *		
 **************************************************************************/

// mp4_decoder.h //

#ifndef _MP4_DECODER_H_
#define _MP4_DECODER_H_

#define MODE_INTER                      0
#define MODE_INTER_Q                    1
#define MODE_INTER4V                    2
#define MODE_INTRA                      3
#define MODE_INTRA_Q                    4
#define MODE_INTER4V_Q                  5

#define ESCAPE                          7167
#define ESCAPE_INDEX                    102

// this is necessary for the max resolution (juice resolution)

#define MBC                             45
#define MBR                             36

#endif // _MP4_DECODER_H_

/***/

extern void get_mp4picture (unsigned char *bmp, int render,int Extra);
extern void initdecoder ();
extern void closedecoder ();
