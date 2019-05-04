 /*This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The GPL can be found at: http://www.gnu.org/copyleft/gpl.html						*
 *																						*
 * Authors:																					*
 *          Marc Dukette      
 *			Pedro Mateu
 **************************************************************************************/


#include "portab.h"
#include "yuv2rgb.h"


/*
void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, int width_y, int height_y,
	int stride_out,int Dither) 
{ 
	int y;
	unsigned int* pus_out=(unsigned int*)puc_out;
	unsigned int* pus_out2	= (unsigned int*) (puc_out + (stride_out<<1));
	unsigned int stride_diff = ((stride_out<<2) - (width_y<<1))>>2; // expressed in bytes

	for (y=height_y>>1; y; y--) {
		register uint8_t *py, *py2, *pu, *pv;
		register int x;
		uint32_t tmp;

		py = puc_y;
		py2 = puc_y + stride_y;
		pu = puc_u;
		pv = puc_v;
		for (x=width_y>>1; x; x--) {
			tmp = *(pu++);
			tmp |= *(py++) << 8;
			tmp |= *(pv++) << 16;
			tmp |= *(py++) << 24;
			//*pus_out=tmp;
			sq_cpy(pus_out,&tmp,4);

			tmp &= 0x00FF00FF;
			tmp |= *(py2++) << 8;
			tmp |= *(py2++) << 24;
			*pus_out2=tmp;
			sq_cpy(pus_out2,&tmp,4);
			pus_out ++;
			pus_out2 ++;
		}

		puc_y += stride_y<<1;
		puc_u += stride_uv;
		puc_v += stride_uv;

		pus_out += stride_diff;
		pus_out2 += stride_diff;
	}

}
*/

void yuv2rgb_565(uint8_t *y, int stride_y, 
uint8_t *cb, uint8_t *cr, int stride_uv, 
uint8_t *puc_out, int w, int h,int stride_dest, int Dither) //,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{
    int i,j;
    unsigned int *texp;
	unsigned int p1,p2;
    texp = (unsigned int*) puc_out; 

    for(j =  h>>1; j ; j--) 
	{
		for(i = 0; i < w>>1; i++) 
		{
			p1=(*(cb) | (((unsigned short)*(y++))<<8));
			p2=(*(cr) | (((unsigned short)*(y++))<<8));

			texp[i] = p1|(p2<<16); 
			p1=(*(cb++) | (((unsigned short)*(y+(stride_y)-2))<<8));
			p2=(*(cr++) | (((unsigned short)*(y+(stride_y)-1))<<8));
			texp[i+(stride_dest>>1)]=p1|(p2<<16);

		}
		texp += stride_dest;
		y+=(stride_y<<1)-w;
		cb+=stride_uv-(w>>1);
		cr+=stride_uv-(w>>1);
    }
}

