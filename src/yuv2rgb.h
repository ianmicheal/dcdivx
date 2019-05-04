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

		
#ifdef __cplusplus
extern "C" {
#endif 

#include "portab.h"

#ifndef _YUVRGB_H_
#define _YUVRGB_H_
#ifdef ARM
//#ifdef IPAQ
void (*convert_yuv)(unsigned char *puc_y, int stride_y,
	unsigned char *puc_u, unsigned char *puc_v, int stride_uv,
	unsigned char *bmp, int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert);

void yuv2rgb_565(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert);
void yuv2rgb_565R(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert);
void yuv2rgb_565Z(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert);
void yuv2rgb_565RZ(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert);
/*#else
void (*convert_yuv)(unsigned char *puc_y, int stride_y,
	unsigned char *puc_u, unsigned char *puc_v, int stride_uv,
	unsigned char *bmp, int width_y, int height_y, int Extra, int Dither,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt);

void yuv2rgb_565(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt);
void yuv2rgb_565R(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt);
void yuv2rgb_565Z(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt);
void yuv2rgb_565RZ(
	uint8_t *puc_y, int stride_y, 
  uint8_t *puc_u, 
	uint8_t *puc_v, int stride_uv, 
  uint8_t *puc_out, 
	int width_y, int height_y, int Extra, int Dither,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt);

#endif*/
#else
void (*convert_yuv)(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither);

void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither);
#endif
#endif // _YUVRGB_H_

#ifdef __cplusplus
}
#endif 
