/************************************************************************
 *
 *  getbits.c, bit level routines for tmndecode (H.263 decoder)
 *  Copyright (C) 1995, 1996  Telenor R&D, Norway
 *
 *  Contacts:
 *  Robert Danielsen                  <Robert.Danielsen@nta.no>
 *
 *  Telenor Research and Development  http://www.nta.no/brukere/DVC/
 *  P.O.Box 83                        tel.:   +47 63 84 84 00
 *  N-2007 Kjeller, Norway            fax.:   +47 63 81 00 76
 *
 *  Copyright (C) 1997  University of BC, Canada
 *  Modified by: Michael Gallant <mikeg@ee.ubc.ca>
 *               Guy Cote <guyc@ee.ubc.ca>
 *               Berna Erol <bernae@ee.ubc.ca>
 *
 *  Contacts:
 *  Michael Gallant                   <mikeg@ee.ubc.ca>
 *
 *  UBC Image Processing Laboratory   http://www.ee.ubc.ca/image
 *  2356 Main Mall                    tel.: +1 604 822 4051
 *  Vancouver BC Canada V6T1Z4        fax.: +1 604 822 5949
 *
 ************************************************************************/
/**
*  Copyright (C) 2001 - Project Mayo
 *
 * adapted by Andrea Graziani (Ag)
 *
 * DivX Advanced Research Center <darc@projectmayo.com>
*
**/

#include "mp4_decoder.h"
#include "global.h"

unsigned char *decore_stream;

/* initialize buffer, call once before first getbits or showbits */

void initbits ()
{
	ld->bitcnt = 0;
	ld->rdptr = decore_stream;
}


#define _SWAP(a) ((a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3]) 

unsigned int showbits (int n) 
{ 
	return ((_SWAP(ld->rdptr))>>(32-ld->bitcnt-n))&(0xFFFFFFFF>>(32-n)); 
} 

void flushbits (int n) 
{ 
	ld->bitcnt += n; 
	if (ld->bitcnt >= 8) { 
		ld->rdptr += ld->bitcnt >> 3; 
		ld->bitcnt = (ld->bitcnt&(0x00000007));// % 8;	
	} 
} 


/* return next n bits (right adjusted) */ 
unsigned int getbits (int n) 
{ 
	unsigned int l;
	l=((_SWAP(ld->rdptr))>>(32-ld->bitcnt-n))&(0xFFFFFFFF>>(32-n));
	ld->bitcnt += n; 
	if (ld->bitcnt >= 8) { 
		ld->rdptr += (ld->bitcnt)>>3; 
		ld->bitcnt = (ld->bitcnt&(0x00000007));// % 8;	
	}
	return l;
} 

unsigned int getbits1() 
{ 
	int l;
	l=((ld->rdptr[0])>>(7-ld->bitcnt))&0x001;
	ld->bitcnt += 1; 
	if (ld->bitcnt >= 8) { 
		ld->rdptr += (ld->bitcnt)>>3; 
		ld->bitcnt = (ld->bitcnt&(0x00000007));// % 8;	
	} 
	return l; 
} 

