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
 * John Funnell                                                           *
 * Andrea Graziani                                                        * 
 *                                                                        *  
 * DivX Advanced Research Center <darc@projectmayo.com>                   * 
 *                                                                        *     
 **************************************************************************/

// mp4_vld.c //

#include "mp4_decoder.h"
#include "global.h"
#define TAB
#include "mp4_vld.h"

/**/

int vldTableB19(int last, int run);
int vldTableB20(int last, int run);
int vldTableB21(int last, int level);
int vldTableB22(int last, int level);

int vldTableB16(int code);
int vldTableB17(int code);

/**/

event_t vld_intra_dct() 
{
	event_t event;
	int tab = 0;
	int lmax, rmax;

	tab = vldTableB16(showbits(12));
	if (!tab) { /* bad code */
		event.run   = 
		event.level = 
		event.last  = -1;
		return event;
	} 

	if (tab != ESCAPE) {
		event.run   = (tab >>  6) & 63;
		event.level =  tab        & 63;
		event.last  = (tab >> 12) &  1;
		event.level = getbits1() ? -event.level : event.level;
	} else {
		/* this value is escaped - see para 7.4.1.3 */
		/* assuming short_video_header == 0 */
		switch (showbits(2)) {
			case 0x0 :  /* Type 1 */
			case 0x1 :  /* Type 1 */
				flushbits(1);
				tab = vldTableB16(showbits(12));  /* use table B-16 */
				if (!tab) { /* bad code */
					event.run   = 
					event.level = 
					event.last  = -1;
					return event;
				}
				event.run   = (tab >>  6) & 63;
				event.level =  tab        & 63;
				event.last  = (tab >> 12) &  1;
				lmax = vldTableB19(event.last, event.run);  /* use table B-19 */
				event.level += lmax;
				event.level =  getbits1() ? -event.level : event.level;
				break;
			case 0x2 :  /* Type 2 */
				flushbits(2);
				tab = vldTableB16(showbits(12));  /* use table B-16 */
				if (!tab) { /* bad code */
					event.run   = 
					event.level = 
					event.last  = -1;
					break;
				}
				event.run   = (tab >>  6) & 63;
				event.level =  tab        & 63;
				event.last  = (tab >> 12) &  1;
				rmax = vldTableB21(event.last, event.level);  /* use table B-21 */
				event.run = event.run + rmax + 1;
				event.level = getbits1() ? -event.level : event.level;
				break;
			case 0x3 :  /* Type 3  - fixed length codes */
				flushbits(2);
				event.last  = getbits1();
				event.run   = getbits(6);  /* table B-18 */ 
				getbits1(); /* marker bit */
				event.level = getbits(12); /* table B-18 */
				/* sign extend level... */
				event.level = (event.level & 0x800) ? (event.level | (-1 ^ 0xfff)) : event.level;
				getbits1(); /* marker bit */
				break;
		}
	}

	return event;
}

/**/

event_t vld_inter_dct() 
{
	event_t event;
	int tab =0;
	int lmax, rmax;

	tab = vldTableB17(showbits(12));
	if (!tab) { /* bad code */
		event.run   = 
		event.level = 
		event.last  = -1;
		return event;
	} 
	if (tab != ESCAPE) {
		event.run   = (tab >>  4) & 255;
		event.level =  tab        & 15;
		event.last  = (tab >> 12) &  1;
		event.level = getbits1() ? -event.level : event.level;
	} else {
		/* this value is escaped - see para 7.4.1.3 */
		/* assuming short_video_header == 0 */
		int mode = showbits(2);
		switch (mode) {
			case 0x0 :  /* Type 1 */
			case 0x1 :  /* Type 1 */
				flushbits(1);
				tab = vldTableB17(showbits(12));  /* use table B-17 */
				if (!tab) { /* bad code */
					event.run   = 
					event.level = 
					event.last  = -1;
					return event;
				}
				event.run   = (tab >>  4) & 255;
				event.level =  tab        & 15;
				event.last  = (tab >> 12) &  1;
				lmax = vldTableB20(event.last, event.run);  /* use table B-20 */
				event.level += lmax;
				event.level = getbits1() ? -event.level : event.level;
				break;
			case 0x2 :  /* Type 2 */
				flushbits(2);
				tab = vldTableB17(showbits(12));  /* use table B-16 */
				if (!tab) { /* bad code */
					event.run   = 
					event.level = 
					event.last  = -1;
					break;
				}
				event.run   = (tab >>  4) & 255;
				event.level =  tab        & 15;
				event.last  = (tab >> 12) &  1;
				rmax = vldTableB22(event.last, event.level);  /* use table B-22 */
				event.run = event.run + rmax + 1;
				event.level = getbits1() ? -event.level : event.level;
				break;
			case 0x3 :  /* Type 3  - fixed length codes */
				flushbits(2);
				event.last  = getbits1();
				event.run   = getbits(6);  /* table B-18 */ 
				getbits1(); /* marker bit */
				event.level = getbits(12); /* table B-18 */
				/* sign extend level... */
				event.level = (event.level & 0x800) ? (event.level | (-1 ^ 0xfff)) : event.level;
				getbits1(); /* marker bit */
				break;
		}
	}

	return event;
}

// Table B-19 -- ESCL(a), LMAX values of intra macroblocks 

int vldTableB19(int last, int run) {
	if (!last){ /* LAST == 0 */
		if (run ==  0) return 27;
		else if (run ==  1) return 10;
		else if (run ==  2) return  5;
		else if (run ==  3) return  4;
		else if (run <=  7) return  3;
		else if (run <=  9) return  2;
		else if (run <= 14) return  1;
		else return  0; 
	} else {    /* LAST == 1 */
		if (run ==  0) return  8;
		else if (run ==  1) return  3;
		else if (run <=  6) return  2;
		else if (run <= 20) return  1;
		else return  0; 
	}
}

/**/

// Table B-20 -- ESCL(b), LMAX values of inter macroblocks 

int vldTableB20(int last, int run) {
	if (!last){ /* LAST == 0 */
		if (run ==  0) return 12;
		else if (run ==  1) return  6;
		else if (run ==  2) return  4;
		else if (run <=  6) return  3;
		else if (run <= 10) return  2;
		else if (run <= 26) return  1;
		else return  0; 
	} else {    /* LAST == 1 */
		if (run ==  0) return  3;
		else if (run ==  1) return  2;
		else if (run <= 40) return  1;
		return  0; 
	}
}

/**/

// Table B-21 -- ESCR(a), RMAX values of intra macroblocks 

int vldTableB21(int last, int level) {
	if (!last){ /* LAST == 0 */
		if (level ==  1) return 14;
		else if (level ==  2) return  9;
		else if (level ==  3) return  7;
		else if (level ==  4) return  3;
		else if (level ==  5) return  2;
		else if (level <= 10) return  1;
		else return  0; 
	} else {    /* LAST == 1 */
		if (level ==  1) return  20;
		else if (level ==  2) return  6;
		else if (level ==  3) return  1;
		else return  0;
	}
}

/**/

// Table B-22 -- ESCR(b), RMAX values of inter macroblocks 

int vldTableB22(int last, int level) {
	if (!last){ /* LAST == 0 */
		if (level ==  1) return 26;
		else if (level ==  2) return 10;
		else if (level ==  3) return  6;
		else if (level ==  4) return  2;
		else if (level <=  6) return  1;
		else return  0;
	} else {    /* LAST == 1 */
		if (level ==  1) return  40;
		else if (level ==  2) return  1;
		else return  0;
	}
}




int tableB1x_1_length(int code);
int tableB1x_2_length(int code);
int tableB1x_3_length(int code);

int vldTableB16(int code) {
	int value,length;
	if (code >= 512) {
		code = (code >> 5) - 16;
		value = tableB16_1[code];
		length = tableB1x_1_length(code);
	} else if (code >= 128) {
		code = (code >> 2) - 32;
		value = tableB16_2[code];
		length = tableB1x_2_length(code);
	} else if (code >= 8) {
		code = code - 8;
		value = tableB16_3[code];
		length = tableB1x_3_length(code);
	} else {
		/* invalid Huffman code */
		return 0;
	}
	flushbits(length);
	return value;
}

/**/

int vldTableB17(int code) {
	int value,length;
	if (code >= 512) {
		code = (code >> 5) - 16;
		value = tableB17_1[code];
		length = tableB1x_1_length(code);
	} else if (code >= 128) {
		code = (code >> 2) - 32;
		value = tableB17_2[code];
		length = tableB1x_2_length(code);
	} else if (code >= 8) {
		code = code - 8;
		value = tableB17_3[code];
		length = tableB1x_3_length(code);
	} else {
		/* invalid Huffman code */
		return 0;
	}
	flushbits(length);
	return value;
}


int tableB1x_2_length(int code){
	if (code>43) return 8;
	else if (code>1) return 9;
	else return 10;
}

int tableB1x_1_length(int code){
	if (code>95) return 4;
	else if (code>79) return 3;
	else if (code>47) return 2;
	else if (code>39) return 4;
	else if (code>27) return 5;
	else if (code>7) return 6;
	else return 7;
}

int tableB1x_3_length(int code){
	if (code>87) return 7;
	else if (code>71) return 12;
	else if (code>55) return 11;
	else if (code>7) return 10;
	else return 11;
}
