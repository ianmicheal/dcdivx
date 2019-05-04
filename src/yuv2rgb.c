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

void * sq_cpy(void *dest, void *src, int n);

#if defined(GAPI)||(defined(_USEVIDBUFF))
#ifdef MIPS
#ifdef HPC
	#define DisplayWidth 640
	#define DisplayHeight 480
	#define VisibleWidth 640
	#define VisibleHeight 480
#else
	#define DisplayWidth 256
	#define DisplayHeight 320
	#define VisibleWidth 240
	#define VisibleHeight 320
#endif
#else
#ifdef IPAQ
	#define DisplayWidth 320
	#define DisplayHeight 240
	#define VisibleWidth 320
	#define VisibleHeight 240
#else
#ifdef HPC
	#define VisibleWidth 640
	#define DisplayWidth 640
	#define VisibleHeight 240
	#define DisplayHeight 240
#else
	#define VisibleWidth 240
	#define DisplayWidth 240
	#define VisibleHeight 320
	#define DisplayHeight 320
#endif
#endif
#endif
#else
#ifdef HPC
	#define VisibleWidth 640
	#define DisplayWidth 640
	#define VisibleHeight 240
	#define DisplayHeight 240
#else
	#define VisibleWidth 240
	#define DisplayWidth 240
	#define VisibleHeight 320
	#define DisplayHeight 320
#endif
#endif


#define _S(a) (a)<0 ? 0 : (a)>255? 255:(a) 


#define _Ps565(r,g,b) ( ((r & 0xF8) >> 3) | (((g & 0xFC) << 3)) | (((b & 0xF8) << 8)))
//#define _Ps565(_r,_g,_b) ((((_r)&0x1f)<<11)|(((_b)&0x3f)<<5)|((_g)&0x1f))


#ifdef MIPS

void yuv2rgb_565( 
uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, 
uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, 
int width_y, int height_y,int stride_dest, int Dither) 
{ 
//YUV2RGB MIPS ASM v1.1 
//v1.2->Crop support added 
//v1.1->pequeño aumento de velocidad
// 
__asm ( //creamos nueva pila y salvamos registros 
  "addiu sp,sp,-36;" 
  "sw  $18,20(sp);" 
  "sw  $17,16(sp);" 
  "sw  $16,12(sp);" 
 
  //calculamos stride_uv 
 
  "srl $14,$5,1;" 
 
  //cargamos todas las variables necesarias 
  "lw  $18,68(sp);"//cargamos stride_dest 
  "lw  $2,64(sp);" 
  "lw  $13,60(sp);" 
  "lw  $25,56(sp);" 
  "lbu $11,0($6);" 
  "lbu $12,0($7);" 
  "lbu $10,0($4);" 
 
  "sll $18,$18,1;");//para poder usar stride_dest como pixels 
__asm( // crop x
 
  "addi $8,$13,-240;"
  "blez $8,no_x_crop;"
  "addi $13,$0,240;"
  
  // crop y
  "no_x_crop:addi $8,$2,-320;"
  "blez $8,no_y_crop;"
  "addi $2,$0,320;"
    
   
  //diferencias de strides 
 
  "no_y_crop:sub $16,$5,$13;" 
  "srl $8,$13,1;" 
  "sub $14,$14,$8;" 
 
  // 
 
  "sll $17,$13,1;" 
  "add $17,$25,$17;" 
 
  //dividimos Y por 2 y comenzamos bucle 
 
  "srl $2,$2,1;" 
 
  "bucle_y:add $15,$4,$5;" 
  "srl $3,$13,1;" 
 
  //bucle x 
 
  "add $17,$17,$18;"//dst_buf2+ 
 
  "bucle_x:addi $11,$11,-144;" 
  "addi $12,$12,-144;" 
);
__asm(
  //r1 
 
  "add $9,$10,$11;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "srl $24,$9,3;" 
 
  //g1 
 
  "addi $9,$10,-16;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00FC;" 
  "sll $9,$9,3;" 
  "or  $24,$24,$9;" 
 
  //b1 
 
  "add $9,$10,$12;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;"); 
 
__asm( "andi $9,$9,0x00F8;" 
  "lbu $10,1($4);"//cargamos aqui para no tener interbloqueo 
 
  "sll $9,$9,8;" 
  "or  $24,$24,$9;" 
 
  //primer punto ,ahora procesamos segundo. 
 
  //r2 
 
  "add $9,$10,$11;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;"
  "andi $9,$9,0x00F8;"
  "sll $9,$9,13;" 
  "or  $24,$24,$9;" 
 
  //g2 
 
  "addi $9,$10,-16;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00FC;" 
  "sll $9,$9,19;" 
  "or  $24,$24,$9;" 
 
  //b2 
 
  "add $9,$10,$12;" 
  "lbu $10,0($15);"//cargamos aqui para no tener interbloqueo 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00F8;" 
  "sll $9,$9,24;" 
  "or  $24,$24,$9;" 
 
  //presentamos el punto 
 
  "sw  $24,0($25);" 
); 
 
__asm( 
  //ahora procesamos la segunda línea 
 
  //f2r1 
 
  "add $9,$10,$11;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "srl $24,$9,3;" 
 
  //f2g1 
 
  "addi $9,$10,-16;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00FC;" 
  "sll $9,$9,3;" 
  "or  $24,$24,$9;" 
 
  //f2b1 
 
  "add $9,$10,$12;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00F8;" 
  "lbu $10,1($15);"//cargamos aqui para no tener interbloqueo 
 
  "sll $9,$9,8;" 
  "or  $24,$24,$9;"); 
 
  //primer punto ,ahora procesamos segundo. 
 
  //f2r2 
 
__asm( "add $9,$10,$11;" 
  "lbu $11,1($6);" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;"
  "andi $9,$9,0x00F8;"
  "sll $9,$9,13;" 
  "or  $24,$24,$9;" 
 
  //f2g2 
 
  "addi $9,$10,-16;" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00FC;" 
  "sll $9,$9,19;" 
  "or  $24,$24,$9;" 
 
  //f2b2 
 
  "add $9,$10,$12;" 
  "lbu $12,1($7);" 
  "sra $8,$9,31;" 
  "srlv $9,$9,$8;" 
  "andi $9,$9,0x00F8;" 
 
  "sll $9,$9,24;" 
  "lbu $10,2($4);" 
  "or  $24,$24,$9;" 
 
  //presentamos el punto 
 
  "sw  $24,0($17);" );
 
  //incrementamos todos los punteros y volvemos al bucle 
__asm(
  "addi $4,$4,+2;" 
  "addi $15,$15,+2;" 
  "addi $6,$6,+1;" 
  "addi $7,$7,+1;" 
  "addi $3,$3,-1;" 
  "addi $25,$25,+4;" 
  "addi $17,$17,+4;" 
 
  "bgtz $3,bucle_x;" 
 
  "add $4,$15,$16;" 
 
  "addu $6,$6,$14;" 
  "addu $7,$7,$14;" 
 
  //cargamos aqui para no tener interbloqueo 
  "lbu $11,0($6);" 
  "lbu $10,0($4);" 
  "lbu $12,0($7);" 
 
  "add $25,$25,$18;"//dest+stride_dest 
  "add $25,$25,$18;" 
  "add $17,$17,$18;" 
 
  "addi $2,$2,-1;" 
  "sll $8,$13,1;" 
  "add $25,$25,$8;" 
  "add $17,$17,$8;" 
 
  "bgtz $2,bucle_y;" 
 
  //restauramos registros, la pila y finalizamos 
  "lw  $16,12(sp);" 
  "lw  $17,16(sp);" 
  "lw  $18,20(sp);" 
  "addiu sp,sp,+36;" 
 
);} 
 
void yuv2rgb_565Z(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int stride_dest, int Dither) 
{ 
// yuv2rgb 4xzoom.32bits 
// v0.2->some optimizations+horizontal crop 
// v0.3->precise bilinear+some optimizations
// v0.4a->removed precise filtering+save word opt.
//  v0.5a->fixed big bug. Precise bilinear only on VR4122.
 
__asm ( "addiu sp,sp,-8;" 
  "sw  $16,0(sp);" 
  "sw  $17,4(sp);"
 
  //cargamos todas las variables 
 
  "lw  $9,40(sp);" 
  "lw  $2,36(sp);" 
  "lw  $3,32(sp);" 
  "lw  $25,28(sp);" 
 
  //recortamos video x_crop
  
  "addi $14,$3,-120;" 
  "blez $14,no_recorte_x;" 
  "addi $3,$0,+120;" 
   
  "no_recorte_x:"
  
  //recortamos video y_crop
  
  "addi $14,$2,-160;" 
  "blez $14,no_recorte_y;" 
  "addi $2,$0,+160;" 
    
  //diferencias de strides: 
  
  "no_recorte_y:srl $8,$5,1;" 
  "sub $5,$5,$3;" 
  
  //
 
  "sll $9,$9,1;" 
  
  //preparando segunda linea
 
  "sll $14,$3,1;"
  "add $17,$25,$9;"
  "add $17,$17,$14;"
 
  //
 
  "bucle_y1:lbu $12,0($7);" 
  "lbu $11,0($6);" 
  "lbu $10,0($4);" 
  "add $16,$3,$0;" 
  "addi $11,$11,-128;" 
  "addi $12,$12,-128;"
  "addi $10,$10,-16;"
  
  //r1 
  
  "add $14,$10,$11;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "srl $13,$15,3;" 
  
  //g1 
  
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $14,$15,0x00FC;" 
  "sll $14,$14,3;" 
  "or  $13,$13,$14;" 
  
  //b1 
  
  "add $14,$10,$12;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $14,$15,0x00F8;" 
  "sll $14,$14,8;" 
  "or  $13,$13,$14;" 
  
  "lbu $10,1($4);" 
  "addi $4,$4,+1;" 
//  "addi $16,$16,-1;"
  "addi $10,$10,-16;"
  ); 
 
__asm( "bucle_x1:add $14,$10,$11;"  
  "sra $15,$14,31;"
  "srlv $24,$14,$15;"
  "srl $24,$24,3;" 
 
  //g3 
 
  "sra $15,$10,31;" 
  "srlv $15,$10,$15;"
  "andi $15,$15,0x00FC;" 
  "sll $14,$15,3;" 
  "or  $24,$24,$14;" 
 
  //b3 
 
  "add $14,$10,$12;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $15,$15,0x00F8;" 
  "sll $14,$15,8;" 
  "or  $24,$24,$14;" 
#if (_WIN32_WCE < 300) 
  //calculamos el punto interpolado 
 
  "srl $14,$13,1;"  //div por 2 p1 
  "srl $15,$24,1;"  //y p2 
 
  "andi $14,$14,0x7BEF;" //los enmascaramos 
  "andi $15,$15,0x7BEF;" 
 
  "addu $13,$14,$15;"  //y los sumamos 
 
#else
  //calculamos el punto interpolado 
 
  "andi $10,$13,0x0821;"//
  "and $10,$10,$24;"//calcula un valor medio perfecto
 
  "srl $14,$13,1;"//div por 2 p1 
  "srl $15,$24,1;"//y p2 
 
  "andi $14,$14,0x7BEF;"//los enmascaramos 
  "andi $15,$15,0x7BEF;" 
 
  "addu $13,$14,$15;"//y los sumamos 
  "addu $13,$13,$10;"
#endif  
  //guardamos puntos 
    
  "sll $15,$24,16;"
  "or  $13,$13,$15;"
 
  "sw  $13,0($25);"
  "sw  $13,0($17);"
     
  //cambiamos de lugar ultimo punto 
  
  "add $13,$24,$0;" 
  
  //incrementamos punteros 
  
  "lbu $10,0($4);"  //cargamos nuevo puc_y 
  "addi $4,$4,+1;" 
  "addi $16,$16,-1;" 
  "sll $14,$16,31;" 
  "addi $25,$25,+4;" 
  "addi $17,$17,+4;"
 
  "addi $10,$10,-16;" //*
     
  "beq $14,$0,par_x;" 
  "addi $6,$6,+1;" 
  "addi $7,$7,+1;" 
  "lbu $12,0($7);" 
  "lbu $11,0($6);" 
  "addi $12,$12,-128;" 
  "addi $11,$11,-128;"
  "par_x:bgtz $16,bucle_x1;"); 
 
__asm( //aumentamos strides: 
  "sll $14,$2,31;" 
  "add $4,$4,$5;" 
  "addi $4,$4,-1;"
  "srl $15,$3,1;" 
  "sub $6,$6,$15;" 
  "sub $7,$7,$15;" 
    
  "beq $14,$0,par_y;" 
  "add $6,$6,$8;" 
  "add $7,$7,$8;" 
 
  //aumentamos stride_dest 
 
  "par_y:addi  $2,$2,-1;" 
  
  "sll $14,$3,1;"
  "sub $17,$17,$14;"
  "add $25,$17,$9;"
 
  "add $17,$25,$9;"
  "add $17,$17,$14;"
  
  "bgtz $2,bucle_y1;" 
 
  "lw  $16,0(sp);" 
  "lw  $17,4(sp);"
 
  "addiu sp,sp,+8;" ); 
} 
 
 
 

void yuv2rgb_565ZPP(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int stride_dest, int Dither) 
{ 
//yuv2rgb 4xzoom BILINEAR FILTERING. 32bit access 
// v0.1=>base code of 0.2 linear zoom asm +bilinear filtering 
// v0.2=>finally 32bits code
 
__asm ( "addiu sp,sp,-36;" 
  "sw  $16,0(sp);" 
  "sw  $17,4(sp);" 
  "sw  $18,8(sp);" 
  "sw  $19,12(sp);" 
  "sw  $20,16(sp);" 
  "sw  $21,20(sp);"
 
  //cargamos todas las variables 
 
  "lw  $9,68(sp);" 
  "lw  $2,64(sp);" 
  "lw  $3,60(sp);" 
  "lw  $25,56(sp);" 
 
  //recortamos video x_crop
  
  "addi $14,$3,-120;" 
  "blez $14,no_recorte_xzpp;" 
  "addi $3,$0,+120;" 
   
  "no_recorte_xzpp:"
  
  //recortamos video y_crop
  
  "addi $14,$2,-160;" 
  "blez $14,no_recorte_yzpp;" 
  "addi $2,$0,+160;" 
 
  //diferencias de strides: 
 
  "no_recorte_yzpp:srl $8,$5,1;" 
 
  "add $17,$5,$4;"//y tomamos el primer puntero 
 
  "sub $5,$5,$3;" 
  
  //
 
  "sll $9,$9,1;" 
  
  //preparando segunda linea
 
  "sll $14,$3,1;"
  "add $21,$25,$9;"
  "add $21,$21,$14;"
   
  //
 
  "bucle_y2:lbu $12,0($7);" 
  "lbu $11,0($6);" 
  "lbu $10,0($4);" 
  "lbu $18,0($17);"//tomamos puc_y de la siguiente linea 
  "add $16,$3,$0;" 
  "addi $11,$11,-144;" 
  "addi $12,$12,-144;"); 
 
  //r1 
 
__asm( "add $14,$10,$11;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "srl $13,$15,3;" 
  
  //g1 
 
  "addi $14,$10,-16;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $14,$15,0x00FC;" 
  "sll $14,$14,3;" 
  "or  $13,$13,$14;" 
 
  //b1 
  
  "add $14,$10,$12;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $14,$15,0x00F8;" 
  "sll $14,$14,8;" 
  "or  $13,$13,$14;" 
 
  //r2 
 
  "srl $10,$10,1;" 
  "srl $18,$18,1;" 
  
  "add $18,$18,$10;" 
 
  "add $14,$18,$11;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "srl $19,$15,3;" 
 
  //g2 
 
  "addi $14,$18,-16;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;" 
  "andi $14,$15,0x00FC;" 
  "sll $14,$14,3;" 
  "or  $19,$19,$14;" 
 
  //b2
  
  "add $14,$18,$12;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;" 
  "andi $14,$15,0x00F8;" 
  "sll $14,$14,8;" 
  "or  $19,$19,$14;" 
 
  "lbu $10,1($4);" 
  "lbu $18,1($17);" 
  "addi $4,$4,+1;" 
  "addi $17,$17,+1;"//sumamos al interpolado 
  "addi $16,$16,-1;"); 
 
__asm( "bucle_x2:add $14,$10,$11;" //r3 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "add $24,$15,$0;" 
  "srl $24,$24,3;" 
 
  //g3 
 
  "addi $14,$10,-16;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $15,$15,0x00FC;" 
  "sll $14,$15,3;" 
  "or  $24,$24,$14;" 
 
  //b3 
 
  "add $14,$10,$12;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;"
  "andi $15,$15,0x00F8;" 
  "sll $14,$15,8;" 
  "or  $24,$24,$14;" 
 
  //calculamos el punto interpolado 
 
  "srl $14,$13,1;"//div por 2 p1 
  "srl $15,$24,1;"//y p2 
 
  "andi $14,$14,0x7BEF;"//los enmascaramos 
  "andi $15,$15,0x7BEF;" 
 
  "addu $13,$14,$15;"
  "sll $15,$24,16;"
  "or  $13,$13,$15;"
  "sw  $13,0($25);" 
  );//y los sumamos 
 
  //re2 
__asm( "srl $10,$10,1;" 
  "srl $18,$18,1;" 
 
  "add $18,$18,$10;" 
 
  "add $14,$18,$11;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;" 
  "srl $20,$15,3;" 
  
  //ge2 
  
  "addi $14,$18,-16;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;" 
  "andi $14,$15,0x00FC;" 
  "sll $14,$14,3;" 
  "or  $20,$20,$14;" 
  
  //be2 
  
  "add $14,$18,$12;" 
  "sra $15,$14,31;"
  "srlv $15,$14,$15;" 
  "andi $14,$15,0x00F8;" 
  "sll $14,$14,8;" 
  "or  $20,$20,$14;" 
 
/// 
 
  "srl $14,$19,1;"//div por 2 p1 
  "srl $15,$20,1;"//y p2 
 
  "andi $14,$14,0x7BEF;"//los enmascaramos 
  "andi $15,$15,0x7BEF;" 
 
  "addu $19,$14,$15;"//y los sumamos 
 

  //guardamos puntos 
  
  "sll $15,$20,16;"
  "or  $19,$19,$15;"
 
  "sw  $19,0($21);" 
 
 
  //cambiamos de lugar ultimo punto 
 
  "add $13,$24,$0;" 
  "add $19,$20,$0;" 
); 
 
  //incrementamos punteros 
 
__asm( "lbu $10,0($4);"//cargamos nuevo puc_y 
  "lbu $18,0($17);" 
  "addi $4,$4,+1;" 
  "sll $14,$16,31;" 
  "addi $17,$17,+1;" 
  "addi $16,$16,-1;" 
  "addi $25,$25,+4;" 
  "addi $21,$21,+4;"
  "beq $14,$0,par_x1;" 
  "addi $6,$6,+1;" 
  "addi $7,$7,+1;" 
  "lbu $12,0($7);" 
  "lbu $11,0($6);" 
  "addi $12,$12,-144;" 
  "addi $11,$11,-144;" 
  "par_x1:bgtz $16,bucle_x2;"); 
 
__asm( //aumentamos strides: 
  "sll $14,$2,31;" 
  "add $4,$4,$5;" 
  "add $17,$17,$5;" 
  "srl $15,$3,1;" 
  "sub $6,$6,$15;" 
  "sub $7,$7,$15;" 
  "beq $14,$0,par_1;" 
  "add $6,$6,$8;" 
  "add $7,$7,$8;" 
  
  //aumentamos stride_dest 
  
  "par_1:addi $2,$2,-1;" 
  "sll $14,$3,1;"
  "sub $21,$21,$14;"
  "add $25,$21,$9;"
  "addi $25,$25,4;"
 
  "add $21,$25,$9;"
  "add $21,$21,$14;"
  "bgtz $2,bucle_y2;" 
 
  "lw  $16,0(sp);" 
  "lw  $17,4(sp);" 
  "lw  $18,8(sp);" 
  "lw  $19,12(sp);" 
  "lw  $20,16(sp);" 
  "lw  $21,20(sp);"
  "addiu sp,sp,+36;" ); 
} 


#else


#if (defined(IPAQ)) //||(defined(MIPS))


void yuv2rgb_565D(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither,int Brightness, uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, __int8 slowopt, int xStart, int xEnd, int xStep) 
{ 
//Floyd-Steinberg dithered version of the yuv2rgbHQ
	register int x,y; 
	register int temp_y,temp_Guv,temp_Guv1,temp_Guv2; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	register int quant_error_r=0,quant_error_g=0,quant_error_b=0;
	pus_out = (unsigned int *) (puc_out); 
	for (y=0; y<height_y; y+=2)
	{ 		
		//if (y==lastpel) width_y-=2;
		for (x=xStart;x!=xEnd;x+=(xStep<<1))
		{ 
			if ((!slowopt)||(puc_y[x]-puc_yp[x]||(puc_y[x+stride_y]-puc_yp[x+stride_y])||(puc_u[x>>1]-puc_up[x>>1])||(puc_v[x>>1]-puc_vp[x>>1])))
			{
				unsigned int hvalue;
				temp_y=9576*(puc_y[x]-16+Brightness);
				temp_Guv=-3218*(puc_v[x>>1]-128)-6686*(puc_u[x>>1]-128);
				temp_Guv2=16591*(puc_v[x>>1]-128);
				temp_Guv1=13123*(puc_u[x>>1]-128);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b); 
				hvalue = (unsigned int) _Ps565(r,g,b); 
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				temp_y=9576*(puc_y[x+xStep]-16+Brightness);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b); 
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				pus_out[0] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
				temp_y=9576*(puc_y[x+stride_y]-16+Brightness);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b); 
				hvalue = (unsigned int) _Ps565(r,g,b); 
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				temp_y=9576*(puc_y[x+stride_y+xStep]-16+Brightness);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b); 
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				pus_out[(width_y+stride_dest)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
			}
			pus_out++; 
		} 
		puc_y += stride_y<<1; 
		puc_yp+=stride_y<<1;
		pus_out+=(width_y>>1)+stride_dest;
		puc_u += stride_uv; 
		puc_v += stride_uv; 
		puc_up += stride_uv; 
		puc_vp += stride_uv; 
	} 
} 

void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, __int8 slowopt, int invert) 
{ 
//Floyd-Steinberg dithered version of the yuv2rgbHQ
	register int x,y; 
	register int temp;//,temp_Guv,temp_Guv1,temp_Guv2; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
//	unsigned int lastpel=238;
	int xStart=0,xEnd=width_y,xStep=1;//,lastpel=318;
	if (stride_dest>=0)
	{
		if (height_y>VisibleHeight)
		{
			puc_y+=(height_y-VisibleHeight)*(stride_y>>1);
			puc_u+=(height_y-VisibleHeight)*(stride_uv>>2);
			puc_v+=(height_y-VisibleHeight)*(stride_uv>>2);
			puc_yp+=(height_y-VisibleHeight)*(stride_y>>1);
			puc_up+=(height_y-VisibleHeight)*(stride_uv>>2);
			puc_vp+=(height_y-VisibleHeight)*(stride_uv>>2);
			height_y=VisibleHeight;
		}
		if (width_y>VisibleWidth)
		{
			puc_y+=(width_y-VisibleWidth)>>1;
			puc_u+=(width_y-VisibleWidth)>>2;
			puc_v+=(width_y-VisibleWidth)>>2;
			puc_yp+=(width_y-VisibleWidth)>>1;
			puc_up+=(width_y-VisibleWidth)>>2;
			puc_vp+=(width_y-VisibleWidth)>>2;
			width_y=VisibleWidth;
		}
		if (invert)
		{
			puc_y += (height_y - 1) * stride_y ; 
			puc_u += (height_y/2 - 1) * stride_uv; 
			puc_v += (height_y/2 - 1) * stride_uv; 
			puc_yp += (height_y - 1) * stride_y ; 
			puc_up += (height_y/2 - 1) * stride_uv; 
			puc_vp += (height_y/2 - 1) * stride_uv; 
			stride_y = -stride_y; 
			stride_uv = -stride_uv; 
			xStart=width_y;
			xEnd=0;
			//lastpel=238;
			xStep=-1;
		}
		else
		{
			xEnd=width_y;
		}
	}
	else
	{
		if (width_y>VisibleHeight)
		{
			puc_y+=(width_y-VisibleHeight)>>1;
			puc_u+=(width_y-VisibleHeight)>>2;
			puc_v+=(width_y-VisibleHeight)>>2;
			width_y=VisibleHeight;
		}
		if (height_y>VisibleWidth)
		{
			puc_y+=(height_y-VisibleWidth)*(stride_y>>1);
			puc_u+=(height_y-VisibleWidth)*(stride_uv>>2);
			puc_v+=(height_y-VisibleWidth)*(stride_uv>>2);
			height_y=VisibleWidth;
		}
		xEnd=width_y;
		stride_dest=0;
//		lastpel=318;
	}
	if (Dither) 
	{
		yuv2rgb_565D(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y,stride_dest, Dither,Brightness,puc_yp,puc_up,puc_vp,slowopt,xStart,xEnd,xStep);
		return;
	}
	pus_out = (unsigned int *) (puc_out); 
	for (y=height_y; y!=0; y-=2)
	{ 		
//		if (y==lastpel) width_y-=2;
		for (x=xStart;x!=xEnd;x+=(xStep<<1))
		{ 
			int x2=x>>1;
			if ((!slowopt)||(puc_y[x]-puc_yp[x]||(puc_y[x+stride_y]-puc_yp[x+stride_y])||(puc_u[x>>1]-puc_up[x>>1])||(puc_v[x>>1]-puc_vp[x>>1])))
			{
				unsigned int hvalue;
				temp=puc_y[x]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				hvalue = (unsigned int) _Ps565(r,g,b); 
				temp=puc_y[x+xStep]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				pus_out[0] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
				temp=puc_y[x+stride_y]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				hvalue = (unsigned int) _Ps565(r,g,b); 
				temp=puc_y[x+xStep+stride_y]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				pus_out[(width_y+stride_dest)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
			}
			pus_out++; 
		} 
		puc_y += stride_y<<1; 
		puc_yp+=stride_y<<1;
		pus_out+=(width_y>>1)+stride_dest;
		puc_u += stride_uv; 
		puc_v += stride_uv; 
		puc_up += stride_uv; 
		puc_vp += stride_uv; 
	} 
} 

void yuv2rgb_565ZD(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int xStart, int xEnd, int xStep) 
{ 
//Floyd-Steinberg dithered version of the yuv2rgbHQ
	register int x,y; 
	register signed int temp_y,temp_Guv,next_y,next_Guv; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	register int quant_error_r,quant_error_g,quant_error_b;
	register int quant_error_r2,quant_error_g2,quant_error_b2;
	int we;
//	if ((height_y<<1)>VisibleHeight) height_y=VisibleHeight>>1;
//	if ((width_y<<1)>VisibleWidth) width_y=VisibleWidth>>1;
//	if (nExtra<0) nExtra=0;
	we=width_y+nExtra;
	pus_out = (unsigned int *) puc_out;	//I can´t get it working with hold 
	quant_error_r=quant_error_g=quant_error_b=0;
	quant_error_r2=quant_error_g2=quant_error_b2=0;
	for (y=0; y<height_y; y++) 
	{ 
		quant_error_r2=quant_error_g2=quant_error_b2=6;
		//if (y==119) width_y-=1; //don't want to go outside the border.
		for (x=xStart;x!=xEnd;x+=xStep)
		{ 
			int x2=x>>1;//add tolerance of 1 
			if ((!slowopt)||(puc_y[x]!=puc_yp[x]||(puc_u[x2]!=puc_up[x2])||(puc_v[x2]!=puc_vp[x2])))
			{
				unsigned int hvalue,hvalue2,hvalue3,hvalue4;
				 
				temp_y=9576*(puc_y[x]-16+Brightness);
				temp_Guv=-3218*(puc_v[x2]-128)-6686*(puc_u[x2]-128);
				r = _S(((temp_y+13123*(puc_u[x2]-128))>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+16591*(puc_v[x2]-128))>>13)+ quant_error_b); 
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				hvalue=(unsigned short) _Ps565(r,g,b);
				
				temp_y=9576*(puc_y[x+stride_y]-16+Brightness);

				if (y<<31) 
				{ 
					temp_Guv=-3218*(puc_v[stride_uv+x2]-128)-6686*(puc_u[stride_uv+x2]-128);
					r+= _S(((temp_y+13123*(puc_u[stride_uv+x2]-128))>>13)+ quant_error_r2); 
					g+= _S(((temp_y+temp_Guv)>>13)+quant_error_g2); 
					b+= _S(((temp_y+16591*(puc_v[stride_uv+x2]-128))>>13)+ quant_error_b2); 
				}
				else
				{
					r+= _S(((temp_y+13123*(puc_u[x2]-128))>>13)+ quant_error_r2); 
					g+= _S(((temp_y+temp_Guv)>>13)+quant_error_g2); 
					b+= _S(((temp_y+16591*(puc_v[x2]-128))>>13)+ quant_error_b2); 
				}
				r = (r>>1);
				g = (g>>1);
				b = (b>>1);
				quant_error_r2=(r&0x0F);
				quant_error_g2=(g&0x0F);
				quant_error_b2=(b&0x0F);
				hvalue2=(unsigned short) _Ps565(r,g,b);

				if (x+xStep!=width_y)
				{
					temp_y=9576*(puc_y[x+xStep]-16+Brightness);
					temp_Guv=-3218*(puc_v[(x+xStep)>>1]-128)-6686*(puc_u[(x+xStep)>>1]-128);
					r += _S(((temp_y+13123*(puc_u[(x+xStep)>>1]-128))>>13)+ quant_error_r); 
					g += _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
					b += _S(((temp_y+16591*(puc_v[(x+xStep)>>1]-128))>>13)+ quant_error_b); 
					r = (r>>1);
					g = (g>>1);
					b = (b>>1);
					quant_error_r=(r&0x0F);
					quant_error_g=(g&0x0F);
					quant_error_b=(b&0x0F);
					hvalue3=(unsigned short)_Ps565(r,g,b);
					next_y=9576*(puc_y[x+xStep+stride_y]-16);

					if (y<<31) 
					{ 
						next_Guv=-3218*(puc_v[stride_uv+((x+xStep)>>1)]-128)-6686*(puc_u[stride_uv+((x+xStep)>>1)]-128);
						r += _S(((next_y+13123*(puc_u[stride_uv+((x+xStep)>>1)]-128))>>13)+ quant_error_r2); 
						g += _S(((next_y+next_Guv)>>13)+quant_error_g2); 
						b += _S(((next_y+16591*(puc_v[stride_uv+((x+xStep)>>1)]-128))>>13)+ quant_error_b2); 
					}
					else
					{
						r += _S(((next_y+13123*(puc_u[((x+xStep)>>1)]-128))>>13)+ quant_error_r2); 
						g += _S(((next_y+temp_Guv)>>13)+quant_error_g2); 
						b += _S(((next_y+16591*(puc_v[((x+xStep)>>1)]-128))>>13)+ quant_error_b2); 
					}
					r = (r>>1);
					g = (g>>1);
					b = (b>>1);
					quant_error_r2=(r&0x0F);
					quant_error_g2=(g&0x0F);
					quant_error_b2=(b&0x0F);
					hvalue4=(unsigned short) _Ps565(r,g,b);
				}
				else
				{
					hvalue2=hvalue;
				}
				pus_out[0] =hvalue|(hvalue3<<16);
				pus_out[we>>1]=hvalue2|(hvalue4<<16);
			}
			pus_out++;
		} 
		pus_out+=nExtra;
		puc_y += stride_y; 
		puc_yp += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
			puc_up += stride_uv; 
			puc_vp += stride_uv; 
		} 
	}
} 

void yuv2rgb_565Z(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 

	register int x,y; 
	register signed int temp,tempnextx,tempnexty,tempnextxy; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	int we;
	int xStart=0,xEnd=width_y,xStep=1;//,lastpel=318;
	if ((height_y<<1)>VisibleHeight) height_y=VisibleHeight>>1;
	if ((width_y<<1)>VisibleWidth) width_y=VisibleWidth>>1;
	if (nExtra<0)
	{
		nExtra=0;
	}
	else
	{
		if (invert)
		{
			puc_y += (height_y - 1) * stride_y ; 
			puc_u += (height_y/2 - 1) * stride_uv; 
			puc_v += (height_y/2 - 1) * stride_uv; 
			puc_yp += (height_y - 1) * stride_y ; 
			puc_up += (height_y/2 - 1) * stride_uv; 
			puc_vp += (height_y/2 - 1) * stride_uv; 
			stride_y = -stride_y; 
			stride_uv = -stride_uv; 
			xStart=width_y;
			xEnd=0;
			//lastpel=238;
			xStep=-1;
		}
		else
		{
			xEnd=width_y;
		}
	}
	if (Dither)
	{
		yuv2rgb_565ZD(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y,nExtra, Dither,Brightness, puc_yp,puc_up,puc_vp,slowopt,xStart,xEnd,xStep);
		return;
	}
	we=width_y+nExtra;
	pus_out = (unsigned int *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
//		if (y==119) width_y-=1;
		for (x=xStart;x!=xEnd;x+=xStep)
		{ 
			unsigned int hvalue,hvalue2,hvalue1,hvalue3;
			int x2=x>>1;
			if ((!slowopt)||(puc_y[x]!=puc_yp[x]||(puc_u[x2]!=puc_up[x2])||(puc_v[x2]!=puc_vp[x2])))
			{
			 
				temp=puc_y[x]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				hvalue=(unsigned short) _Ps565(r,g,b);
				tempnexty=puc_y[x+stride_y]-144+Brightness; 
				if (y<<31) 
				{ 
					r = _S((temp+puc_u[x2]+tempnexty+puc_u[stride_uv+x2])/2); 
					g = _S((temp+tempnexty+256)/2); 
					b = _S((temp+puc_v[x2]+tempnexty+puc_v[stride_uv+x2])/2); 
				}
				else
				{
					r = _S((temp+puc_u[x2]+tempnexty+puc_u[x2])/2); 
					g = _S((temp+tempnexty+256)/2); 
					b = _S((temp+puc_v[x2]+tempnexty+puc_v[x2])/2); 
				}
				hvalue1=(unsigned short) _Ps565(r,g,b);
				if ((x+xStep!=width_y))
				{
					tempnextx=puc_y[x+xStep]-144+Brightness; 
					r = _S((temp+puc_u[x2]+tempnextx+puc_u[x2])/2); 
					g = _S((temp+tempnextx+256)/2); 
					b = _S((temp+puc_v[x2]+tempnextx+puc_v[x2])/2); 
					hvalue2=(unsigned short) _Ps565(r,g,b);
					tempnextxy=puc_y[x+xStep+stride_y]-144+Brightness; 

					if (y<<31) 
					{ 
						r = _S((temp+puc_u[x2]+tempnextxy+puc_u[stride_uv+x2])/2); 
						g = _S((temp+tempnextxy+256)/2); 
						b = _S((temp+puc_v[x2]+tempnextxy+puc_v[stride_uv+x2])/2); 
					}
					else
					{
						r = _S((temp+puc_u[x2]+tempnextxy+puc_u[x2])/2); 
						g = _S((temp+tempnextxy+256)/2); 
						b = _S((temp+puc_v[x2]+tempnextxy+puc_v[x2])/2); 
					}
					hvalue3=(unsigned short) _Ps565(r,g,b);
				}
				else
				{
					hvalue2=hvalue3=hvalue;
				}
				pus_out[0] = hvalue|(hvalue2<<16);
				pus_out[we>>1]=hvalue1|(hvalue3<<16);
			}
			pus_out++;
		} 
		pus_out+=nExtra;
		puc_y += stride_y; 
		puc_yp += stride_y; 

		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
			puc_up += stride_uv; 
			puc_vp += stride_uv; 
		} 
	}
//	memcpy(puc_out,hold,320*(height_y<<2));
} 
#else 
#if defined(ARM)

void yuv2rgb_565Z(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 
	register int x,y; 
	register signed int temp,tempnextx,tempnexty,tempnextxy; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	int we;
	if ((height_y<<1)>VisibleHeight) height_y=VisibleHeight>>1;
	if ((width_y<<1)>VisibleWidth) width_y=VisibleWidth>>1;
	if (nExtra<0) nExtra=0;
	we=width_y+nExtra;
	pus_out = (unsigned short *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		for (x=0;x<width_y;x++)
		{ 
			unsigned short hvalue,hvalue2,hvalue1,hvalue3;
			int x2=x>>1;
			if ((!slowopt)||(puc_y[x]!=puc_yp[x]||(puc_u[x2]!=puc_up[x2])||(puc_v[x2]!=puc_vp[x2])))
			{
			 
				temp=puc_y[x]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				hvalue=(unsigned short) _Ps565(r,g,b);
				tempnexty=puc_y[x+stride_y]-144+Brightness; 
				if (y<<31) 
				{ 
					r = _S((temp+puc_u[x2]+tempnexty+puc_u[stride_uv+x2])/2); 
					g = _S((temp+tempnexty+256)/2); 
					b = _S((temp+puc_v[x2]+tempnexty+puc_v[stride_uv+x2])/2); 
				}
				else
				{
					r = _S((temp+puc_u[x2]+tempnexty+puc_u[x2])/2); 
					g = _S((temp+tempnexty+256)/2); 
					b = _S((temp+puc_v[x2]+tempnexty+puc_v[x2])/2); 
				}
				hvalue1=(unsigned short) _Ps565(r,g,b);
				if ((x+1!=width_y))
				{
					tempnextx=puc_y[x+1]-144+Brightness; 
					r = _S((temp+puc_u[x2]+tempnextx+puc_u[x2])/2); 
					g = _S((temp+tempnextx+256)/2); 
					b = _S((temp+puc_v[x2]+tempnextx+puc_v[x2])/2); 
					hvalue2=(unsigned short) _Ps565(r,g,b);
					tempnextxy=puc_y[x+1+stride_y]-144+Brightness; 

					if (y<<31) 
					{ 
						r = _S((temp+puc_u[x2]+tempnextxy+puc_u[stride_uv+x2])/2); 
						g = _S((temp+tempnextxy+256)/2); 
						b = _S((temp+puc_v[x2]+tempnextxy+puc_v[stride_uv+x2])/2); 
					}
					else
					{
						r = _S((temp+puc_u[x2]+tempnextxy+puc_u[x2])/2); 
						g = _S((temp+tempnextxy+256)/2); 
						b = _S((temp+puc_v[x2]+tempnextxy+puc_v[x2])/2); 
					}
					hvalue3=(unsigned short) _Ps565(r,g,b);
				}
				else
				{
					hvalue2=hvalue3=hvalue;
				}
				pus_out[0] = hvalue;
				pus_out[1] = hvalue2;
				pus_out[we]=hvalue1;
				pus_out[we+1]=hvalue3;
			}
			pus_out+=2;
		} 
		pus_out+=nExtra<<1;
		puc_y += stride_y; 
		puc_yp += stride_y; 

		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
			puc_up += stride_uv; 
			puc_vp += stride_uv; 
		} 
	}
} 
/*
void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt) 
{ 
	register int x,y; 
	register signed int temp; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	if (height_y>VisibleHeight)
	{
		puc_y+=(height_y-VisibleHeight)*(stride_y>>1);
		puc_u+=(height_y-VisibleHeight)*(stride_uv>>2);
		puc_v+=(height_y-VisibleHeight)*(stride_uv>>2);
		puc_yp+=(height_y-VisibleHeight)*(stride_y>>1);
		puc_up+=(height_y-VisibleHeight)*(stride_uv>>2);
		puc_vp+=(height_y-VisibleHeight)*(stride_uv>>2);
		height_y=VisibleHeight;
	}
	if (width_y>VisibleWidth)
	{
		puc_y+=(width_y-VisibleWidth)>>1;
		puc_u+=(width_y-VisibleWidth)>>2;
		puc_v+=(width_y-VisibleWidth)>>2;
		puc_yp+=(width_y-VisibleWidth)>>1;
		puc_up+=(width_y-VisibleWidth)>>2;
		puc_vp+=(width_y-VisibleWidth)>>2;
		width_y=VisibleWidth;
	}
	pus_out = (unsigned short *) (puc_out); 

	for (y=0; y<height_y; y++)
	{ 
		for (x=0;x<width_y;x++)
		{ 
			int x2=x>>1;
			if ((!slowopt)||(puc_y[x]!=puc_yp[x]||(puc_u[x2]!=puc_up[x2])||(puc_v[x2]!=puc_vp[x2])))
			{
				temp=puc_y[x]-144; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				pus_out[0] = (unsigned short) _Ps565(r,g,b); 
			}
			pus_out++; 
		} 
		puc_y += stride_y; 
		puc_yp+=stride_y;
		pus_out+=stride_dest;
		if (y<<31)
		{
			puc_u += stride_uv; 
			puc_v += stride_uv; 
			puc_up += stride_uv; 
			puc_vp += stride_uv; 
		}
	} 
} 
*/
void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 
	register int x,y; 
	register signed int temp; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	if (height_y>VisibleHeight)
	{
		puc_y+=(height_y-VisibleHeight)*(stride_y>>1);
		puc_u+=(height_y-VisibleHeight)*(stride_uv>>2);
		puc_v+=(height_y-VisibleHeight)*(stride_uv>>2);
		puc_yp+=(height_y-VisibleHeight)*(stride_y>>1);
		puc_up+=(height_y-VisibleHeight)*(stride_uv>>2);
		puc_vp+=(height_y-VisibleHeight)*(stride_uv>>2);
		height_y=VisibleHeight;
	}
	if (width_y>VisibleWidth)
	{
		puc_y+=(width_y-VisibleWidth)>>1;
		puc_u+=(width_y-VisibleWidth)>>2;
		puc_v+=(width_y-VisibleWidth)>>2;
		puc_yp+=(width_y-VisibleWidth)>>1;
		puc_up+=(width_y-VisibleWidth)>>2;
		puc_vp+=(width_y-VisibleWidth)>>2;
		width_y=VisibleWidth;
	}
	pus_out = (unsigned int *) (puc_out); 
	for (y=0; y<height_y; y+=2)
	{ 		
//		if (y==lastpel) width_y-=2;
		for (x=0;x!=width_y;x+=2)
		{ 
			int x2=x>>1;
			if ((!slowopt)||(puc_y[x]-puc_yp[x]||(puc_y[x+stride_y]-puc_yp[x+stride_y])||(puc_u[x>>1]-puc_up[x>>1])||(puc_v[x>>1]-puc_vp[x>>1])))
			{
				unsigned int hvalue;
				temp=puc_y[x]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				hvalue = (unsigned int) _Ps565(r,g,b); 
				temp=puc_y[x+1]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				pus_out[0] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
				temp=puc_y[x+stride_y]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				hvalue = (unsigned int) _Ps565(r,g,b); 
				temp=puc_y[x+1+stride_y]-144+Brightness; 
				r = _S(temp+puc_u[x2]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x2]); 
				pus_out[(width_y+stride_dest)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
			}
			pus_out++; 
		} 
		puc_y += stride_y<<1; 
		puc_yp+=stride_y<<1;
		pus_out+=(width_y>>1)+stride_dest;
		puc_u += stride_uv; 
		puc_v += stride_uv; 
		puc_up += stride_uv; 
		puc_vp += stride_uv; 
	} 
} 

#else
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
/*
void yuv2rgb_565(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y,int stride_dest, int Dither) //,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 
	register int x,y; 
	register signed int temp; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	pus_out = (unsigned int *) (puc_out); 
	for (y=0; y<height_y; y+=2)
	{ 		
		for (x=0;x!=width_y;x+=2)
		{ 
			int x2=x>>1;
			unsigned int hvalue;
			temp=puc_y[x]-144; 
			r = _S(temp+puc_u[x2]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x2]); 
			hvalue = (unsigned int) _Ps565(r,g,b); 
			temp=puc_y[x+1]-144; 
			r = _S(temp+puc_u[x2]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x2]); 
			pus_out[0] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
			temp=puc_y[x+stride_y]-144; 
			r = _S(temp+puc_u[x2]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x2]); 
			hvalue = (unsigned int) _Ps565(r,g,b); 
			temp=puc_y[x+1+stride_y]-144; 
			r = _S(temp+puc_u[x2]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x2]); 
			pus_out[(width_y+stride_dest)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 
			pus_out++; 
		} 
		puc_y += stride_y<<1; 
		pus_out+=(width_y>>1)+stride_dest;
		puc_u += stride_uv; 
		puc_v += stride_uv; 
	} 
} 
*/
void * sq_set32(void *s, int c, int n);

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

/*
void yuv2rgb_565Z(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither) 
{ 

	register int x,y; 
	register signed int temp; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	int we;
	we=width_y+nExtra;
	pus_out = (unsigned short *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		for (x=0;x<width_y;x++)
		{ 
			temp=puc_y[x]-144; 
			r = _S(temp+puc_u[x>>1]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x>>1]); 
			pus_out[0] = pus_out[1]= pus_out[we] = pus_out[we+1]=(unsigned short) _Ps565(r,g,b); 
			pus_out+=2;

		} 
		pus_out+=nExtra<<1;
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 


void yuv2rgb_565Z(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither) //,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 

	register int x,y; 
	register signed int temp,tempnextx,tempnexty,tempnextxy; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	int we;
	int xStart=0,xEnd=width_y,xStep=1;//,lastpel=318;
	we=width_y+nExtra;
	pus_out = (unsigned int *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		for (x=xStart;x!=xEnd;x+=xStep)
		{ 
			unsigned int hvalue,hvalue2,hvalue1,hvalue3;
			int x2=x>>1;
			temp=puc_y[x]-144; 
			r = _S(temp+puc_u[x2]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x2]); 
			hvalue=(unsigned short) _Ps565(r,g,b);
			tempnexty=puc_y[x+stride_y]-144; 
			if (y<<31) 
			{ 
				r = _S((temp+puc_u[x2]+tempnexty+puc_u[stride_uv+x2])/2); 
				g = _S((temp+tempnexty+256)/2); 
				b = _S((temp+puc_v[x2]+tempnexty+puc_v[stride_uv+x2])/2); 
			}
			else
			{
				r = _S((temp+puc_u[x2]+tempnexty+puc_u[x2])/2); 
				g = _S((temp+tempnexty+256)/2); 
				b = _S((temp+puc_v[x2]+tempnexty+puc_v[x2])/2); 
			}
			hvalue1=(unsigned short) _Ps565(r,g,b);
			if ((x+xStep!=width_y))
			{
				tempnextx=puc_y[x+xStep]-144; 
				r = _S((temp+puc_u[x2]+tempnextx+puc_u[x2])/2); 
				g = _S((temp+tempnextx+256)/2); 
				b = _S((temp+puc_v[x2]+tempnextx+puc_v[x2])/2); 
				hvalue2=(unsigned short) _Ps565(r,g,b);
				tempnextxy=puc_y[x+xStep+stride_y]-144; 

				if (y<<31) 
				{ 
					r = _S((temp+puc_u[x2]+tempnextxy+puc_u[stride_uv+x2])/2); 
					g = _S((temp+tempnextxy+256)/2); 
					b = _S((temp+puc_v[x2]+tempnextxy+puc_v[stride_uv+x2])/2); 
				}
				else
				{
					r = _S((temp+puc_u[x2]+tempnextxy+puc_u[x2])/2); 
					g = _S((temp+tempnextxy+256)/2); 
					b = _S((temp+puc_v[x2]+tempnextxy+puc_v[x2])/2); 
				}
				hvalue3=(unsigned short) _Ps565(r,g,b);
			}
			else
			{
				hvalue2=hvalue3=hvalue;
			}
			pus_out[0] = hvalue|(hvalue2<<16);
			pus_out[we>>1]=hvalue1|(hvalue3<<16);
			pus_out++;
		} 
		pus_out+=nExtra;
		puc_y += stride_y; 

		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	}
}*/
#endif
#endif
#endif

#if defined(IPAQ) //||(defined(MIPS))

void yuv2rgb_565RD(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int xStart, int xEnd, int xStep)
{ 
//Floyd-Steinberg dithered version of the yuv2rgbHQ
	register int x,y; 
	register int temp_y,temp_Guv,temp_Guv2,temp_Guv1; 
	register int quant_error_r,quant_error_g,quant_error_b;
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	int hvalue;
	quant_error_r=quant_error_g=quant_error_b=0;
	for (y=0; y<height_y; y+=2) 
	{ 
		pus_out = (unsigned int *) puc_out;
		pus_out+=y>>1;
//		if (y==lastpel) width_y-=2;
		for (x=xStart;x!=xEnd;x+=(xStep<<1))
		{ 
			if ((!slowopt)||(puc_y[x]-puc_yp[x]||(puc_y[x+xStep]-puc_yp[x+xStep])||(puc_u[x>>1]-puc_up[x>>1])||(puc_v[x>>1]-puc_vp[x>>1])))
			{
				temp_y=9576*(puc_y[x]-16+Brightness);
				temp_Guv=-3218*(puc_v[x>>1]-128)-6686*(puc_u[x>>1]-128);
				temp_Guv2=16591*(puc_v[x>>1]-128);
				temp_Guv1=13123*(puc_u[x>>1]-128);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b);  
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				hvalue=(unsigned int) _Ps565(r,g,b);
				temp_y=9576*(puc_y[x+stride_y]-16+Brightness);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b);  
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);

				pus_out[0] = hvalue|((unsigned int) _Ps565(r,g,b)<<16); 

				temp_y=9576*(puc_y[x+xStep]-16+Brightness);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b);  
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);
				hvalue=(unsigned int) _Ps565(r,g,b);
				temp_y=9576*(puc_y[x+stride_y+xStep]-16+Brightness);
				r = _S(((temp_y+temp_Guv1)>>13)+ quant_error_r); 
				g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
				b = _S(((temp_y+temp_Guv2)>>13)+ quant_error_b);  
				quant_error_r=(r&0x0F);
				quant_error_g=(g&0x0F);
				quant_error_b=(b&0x0F);

				pus_out[(height_y+Extra)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 



			}
			pus_out+=(height_y+Extra);
		} 
		puc_y += stride_y<<1; 
		puc_yp += stride_y<<1; 
		puc_u += stride_uv; 
		puc_v += stride_uv; 

		puc_up += stride_uv; 
		puc_vp += stride_uv; 
	} 
} 

void yuv2rgb_565R(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int Extra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 
//Floyd-Steinberg dithered version of the yuv2rgbHQ
	register int x,y; 
	register int temp; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	int hvalue;
	int xStart=0,xEnd=0,xStep=-1;//,lastpel=318;
	if (Extra<0)
	{
		if (height_y>VisibleHeight)
		{
			puc_y+=(height_y-VisibleHeight)*(stride_y>>1);
			puc_u+=(height_y-VisibleHeight)*(stride_uv>>2);
			puc_v+=(height_y-VisibleHeight)*(stride_uv>>2);
			height_y=VisibleHeight;
		}
		if (width_y>VisibleWidth)
		{
			puc_y+=(width_y-VisibleWidth)>>1;
			puc_u+=(width_y-VisibleWidth)>>2;
			puc_v+=(width_y-VisibleWidth)>>2;
			width_y=VisibleWidth;
		}
		xStart=width_y;
		Extra=0;
	}
	else
	{
		if (width_y>VisibleHeight)
		{
			puc_y+=(width_y-VisibleHeight)>>1;
			puc_u+=(width_y-VisibleHeight)>>2;
			puc_v+=(width_y-VisibleHeight)>>2;
			puc_yp+=(width_y-VisibleHeight)>>1;
			puc_up+=(width_y-VisibleHeight)>>2;
			puc_vp+=(width_y-VisibleHeight)>>2;
			width_y=VisibleHeight;
		}
		if (height_y>VisibleWidth)
		{
			puc_y+=(height_y-VisibleWidth)*(stride_y>>1);
			puc_u+=(height_y-VisibleWidth)*(stride_uv>>2);
			puc_v+=(height_y-VisibleWidth)*(stride_uv>>2);
			puc_yp+=(height_y-VisibleWidth)*(stride_y>>1);
			puc_up+=(height_y-VisibleWidth)*(stride_uv>>2);
			puc_vp+=(height_y-VisibleWidth)*(stride_uv>>2);
			height_y=VisibleWidth;
		}
		if (!invert)
		{
			puc_y += (height_y - 1) * stride_y ; 
			puc_u += (height_y/2 - 1) * stride_uv; 
			puc_v += (height_y/2 - 1) * stride_uv; 
			puc_yp += (height_y - 1) * stride_y ; 
			puc_up += (height_y/2 - 1) * stride_uv; 
			puc_vp += (height_y/2 - 1) * stride_uv; 
			stride_y = -stride_y; 
			stride_uv = -stride_uv; 
			xStart=0;
			xEnd=width_y;
			//lastpel=238;
			xStep=1;
		}
		else
		{
			xStart=width_y;
		}
	}
	if (Dither)
	{
		yuv2rgb_565RD(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y,Extra, Dither, Brightness, puc_yp,puc_up,puc_vp,slowopt,xStart,xEnd,xStep);
		return;
	}
	for (y=0; y<height_y; y+=2) 
	{ 
		pus_out = (unsigned int *) puc_out;
		pus_out+=y>>1;
		for (x=xStart;x!=xEnd;x+=(xStep<<1))
		{ 
			if ((!slowopt)||(puc_y[x]-puc_yp[x]||(puc_y[x+xStep]-puc_yp[x+xStep])||(puc_u[x>>1]-puc_up[x>>1])||(puc_v[x>>1]-puc_vp[x>>1])))
			{
				temp=puc_y[x]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 
				hvalue=(unsigned int) _Ps565(r,g,b);
				temp=puc_y[x+stride_y]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 

				pus_out[0] = hvalue|((unsigned int) _Ps565(r,g,b)<<16); 

				temp=puc_y[x+xStep]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 
				hvalue=(unsigned int) _Ps565(r,g,b);
				temp=puc_y[x+stride_y+xStep]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 

				pus_out[(height_y+Extra)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 

			}
			pus_out+=(height_y+Extra);
		} 
		puc_y += stride_y<<1; 
		puc_yp += stride_y<<1; 
		puc_u += stride_uv; 
		puc_v += stride_uv; 

		puc_up += stride_uv; 
		puc_vp += stride_uv; 
	} 
} 
void yuv2rgb_565RZD(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int xStart, int xEnd, int xStep) 
{ 

	register int x,y; 
	register signed int temp_y,temp_Guv; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	unsigned int hvalue,hvalue2,hvalue3,hvalue4;
	int quant_error_r,quant_error_g,quant_error_b,quant_error_r2,quant_error_g2,quant_error_b2=0;
//	int xStart,xEnd=0,xStep=-1;
//	if ((width_y<<1)>VisibleHeight) width_y=VisibleHeight>>1;
//	if ((height_y<<1)>VisibleWidth) height_y=VisibleWidth>>1;
//	xStart=width_y;
//	if (nExtra>=0) 
//	{ 
//		puc_y += (height_y - 1) * stride_y ; 
//		puc_u += (height_y/2 - 1) * stride_uv; 
//		puc_v += (height_y/2 - 1) * stride_uv; 
//		stride_y = -stride_y; 
//		stride_uv = -stride_uv; 
//		xStart=0;
//		xEnd=width_y;
//		xStep=1;
//	} 
//	else
//	{
//		nExtra=0;
//	}
	pus_out = (unsigned int *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		pus_out = (unsigned int *) puc_out;
		pus_out+=y;
		quant_error_r2,quant_error_g2,quant_error_b2=6;
		for (x=xStart;x!=xEnd;x+=xStep)
		{ 
			temp_y=9576*(puc_y[x]-16+Brightness);
			temp_Guv=-3218*(puc_v[x>>1]-128)-6686*(puc_u[x>>1]-128);
			r = _S(((temp_y+13123*(puc_u[x>>1]-128))>>13)+ quant_error_r); 
			g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
			b = _S(((temp_y+16591*(puc_v[x>>1]-128))>>13)+ quant_error_b); 
			quant_error_r=(r&0x0F);
			quant_error_g=(g&0x0F);
			quant_error_b=(b&0x0F);
			hvalue=(unsigned int) _Ps565(r,g,b);
			temp_y=9576*(puc_y[x]-16+Brightness);
			temp_Guv=-3218*(puc_v[x>>1]-128)-6686*(puc_u[x>>1]-128);
			r = _S(((temp_y+13123*(puc_u[x>>1]-128))>>13)+ quant_error_r); 
			g = _S(((temp_y+temp_Guv)>>13)+quant_error_g); 
			b = _S(((temp_y+16591*(puc_v[x>>1]-128))>>13)+ quant_error_b); 
			quant_error_r=(r&0x0F);
			quant_error_g=(g&0x0F);
			quant_error_b=(b&0x0F);
			hvalue2=(unsigned int) _Ps565(r,g,b);
			if (/*Dither&&*/(x+xStep!=xEnd))
			{
				temp_y=9576*(puc_y[x+xStep]-16+Brightness);
				temp_Guv=-3218*(puc_v[(x+xStep)>>1]-128)-6686*(puc_u[(x+xStep)>>1]-128);
				r += _S(((temp_y+13123*(puc_u[(x+xStep)>>1]-128))>>13)+ quant_error_r2); 
				g += _S(((temp_y+temp_Guv)>>13)+quant_error_g2); 
				b += _S(((temp_y+16591*(puc_v[(x+xStep)>>1]-128))>>13)+ quant_error_b2); 
				r = (r>>1);
				g = (g>>1);
				b = (b>>1);
				quant_error_r2=(r&0x0F);
				quant_error_g2=(g&0x0F);
				quant_error_b2=(b&0x0F);
				hvalue3=(unsigned int) _Ps565(r,g,b);
				r += _S(((temp_y+13123*(puc_u[(x+xStep)>>1]-128))>>13)+ quant_error_r2); 
				g += _S(((temp_y+temp_Guv)>>13)+quant_error_g2); 
				b += _S(((temp_y+16591*(puc_v[(x+xStep)>>1]-128))>>13)+ quant_error_b2); 
				r = (r>>1);
				g = (g>>1);
				b = (b>>1);
				quant_error_r2=(r&0x0F);
				quant_error_g2=(g&0x0F);
				quant_error_b2=(b&0x0F);
				hvalue4=(unsigned int) _Ps565(r,g,b);
			}
			else
			{
				hvalue2=hvalue;
			}
			pus_out[0] =hvalue|(hvalue2<<16); 
			pus_out[(height_y+nExtra)>>1] = hvalue3|(hvalue4<<16); 
			pus_out+=((height_y+nExtra));

		} 
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 



void yuv2rgb_565RZ(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 

	register int x,y; 
	register signed int temp,tempnext; 
	unsigned int *pus_out; 
	register unsigned int r,g,b; 
	unsigned short hvalue,hvalue2;
	int xStart,xEnd=0,xStep=-1;
	if ((width_y<<1)>VisibleHeight) width_y=VisibleHeight>>1;
	if ((height_y<<1)>VisibleWidth) height_y=VisibleWidth>>1;
	xStart=width_y;
	if (nExtra>=0&&(!invert)) 
	{ 
		puc_y += (height_y - 1) * stride_y ; 
		puc_u += (height_y/2 - 1) * stride_uv; 
		puc_v += (height_y/2 - 1) * stride_uv; 
		puc_yp += (height_y - 1) * stride_y ; 
		puc_up += (height_y/2 - 1) * stride_uv; 
		puc_vp += (height_y/2 - 1) * stride_uv; 
		stride_y = -stride_y; 
		stride_uv = -stride_uv; 
		xStart=0;
		xEnd=width_y;
		xStep=1;
	} 
	else if (nExtra<0)
	{
		nExtra=0;
	}
	if (Dither)
	{
		yuv2rgb_565RZD(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y,nExtra, Dither,Brightness,puc_yp,puc_up,puc_vp,slowopt,xStart,xEnd,xStep);
		return;
	}
	pus_out = (unsigned int *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		pus_out = (unsigned int *) puc_out;
		pus_out+=y;
		for (x=xStart;x!=xEnd;x+=xStep)
		{ 
			temp=puc_y[x]-144+Brightness; 
			r = _S(temp+puc_u[x>>1]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x>>1]); 
			hvalue=(unsigned int) _Ps565(r,g,b);
			if ((x+xStep!=xEnd))
			{
				tempnext=puc_y[x+xStep]-144+Brightness; 
				r = _S((temp+puc_u[x>>1]+tempnext+puc_u[(x+xStep)>>1])/2); 
				g = _S((temp+tempnext+256)/2); 
				b = _S((temp+puc_v[x>>1]+tempnext+puc_v[(x+xStep)>>1])/2); 
				hvalue2=(unsigned int) _Ps565(r,g,b);
			}
			else
			{
				hvalue2=hvalue;
			}
			pus_out[0]=hvalue|(hvalue<<16); 
			pus_out[(height_y+nExtra)>>1] =hvalue2|(hvalue2<<16); 
			pus_out+=((height_y+nExtra));

		} 
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 

#else
#if defined(ARM)
void yuv2rgb_565R(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int Extra, int Dither,int Brightness ,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert)
{ 

	register int x,y; 
	register signed int temp; 
	unsigned int *pus_out; 
	int hvalue;
	register unsigned int r,g,b; 
	if (Extra<0) Extra=0;
	if (width_y>VisibleHeight)
	{
		puc_y+=(width_y-VisibleHeight)>>1;
		puc_u+=(width_y-VisibleHeight)>>2;
		puc_v+=(width_y-VisibleHeight)>>2;
		puc_yp+=(width_y-VisibleHeight)>>1;
		puc_up+=(width_y-VisibleHeight)>>2;
		puc_vp+=(width_y-VisibleHeight)>>2;
		width_y=VisibleHeight;
	}
	if (height_y>VisibleWidth)
	{
		puc_y+=(height_y-VisibleWidth)*(stride_y>>1);
		puc_u+=(height_y-VisibleWidth)*(stride_uv>>2);
		puc_v+=(height_y-VisibleWidth)*(stride_uv>>2);
		puc_yp+=(height_y-VisibleWidth)*(stride_y>>1);
		puc_up+=(height_y-VisibleWidth)*(stride_uv>>2);
		puc_vp+=(height_y-VisibleWidth)*(stride_uv>>2);
		height_y=VisibleWidth;
	}
	for (y=0; y<height_y; y+=2) 
	{ 
		pus_out = (unsigned int *) puc_out;
		pus_out+=y>>1;
		for (x=width_y-1;x>0;x-=2)
		{ 
			if ((!slowopt)||(puc_y[x]-puc_yp[x]||(puc_y[x-1]-puc_yp[x-1])||(puc_u[x>>1]-puc_up[x>>1])||(puc_v[x>>1]-puc_vp[x>>1])))
			{
				temp=puc_y[x]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 
				hvalue=(unsigned int) _Ps565(r,g,b);
				temp=puc_y[x+stride_y]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 

				pus_out[0] = hvalue|((unsigned int) _Ps565(r,g,b)<<16); 

				temp=puc_y[x-1]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 
				hvalue=(unsigned int) _Ps565(r,g,b);
				temp=puc_y[x+stride_y-1]-144+Brightness; 
				r = _S(temp+puc_u[x>>1]); 
				g = _S(temp+128); 
				b = _S(temp+puc_v[x>>1]); 

				pus_out[(height_y+Extra)>>1] = hvalue|(((unsigned int) _Ps565(r,g,b))<<16); 

			}
			pus_out+=(height_y+Extra);
		} 
		puc_y += stride_y<<1; 
		puc_yp += stride_y<<1; 
		puc_u += stride_uv; 
		puc_v += stride_uv; 

		puc_up += stride_uv; 
		puc_vp += stride_uv; 
	} 
} 

/*
void yuv2rgb_565R(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int Extra, int Dither,int Brightness ,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt)
{ 

	register int x,y; 
	register signed int temp; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	if (Extra<0) Extra=0;
	if (width_y>VisibleHeight)
	{
		puc_y+=(width_y-VisibleHeight)>>1;
		puc_u+=(width_y-VisibleHeight)>>2;
		puc_v+=(width_y-VisibleHeight)>>2;
		width_y=VisibleHeight;
	}
	if (height_y>VisibleWidth)
	{
		puc_y+=(height_y-VisibleWidth)*(stride_y>>1);
		puc_u+=(height_y-VisibleWidth)*(stride_uv>>2);
		puc_v+=(height_y-VisibleWidth)*(stride_uv>>2);
		height_y=VisibleWidth;
	}
	pus_out = (unsigned short *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		pus_out = (unsigned short *) puc_out;
		pus_out+=y;
		for (x=width_y-1;x!=0;x--)
		{ 
			temp=puc_y[x]-144; 
			r = _S(temp+puc_u[x>>1]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x>>1]); 
			pus_out[0] = (unsigned short) _Ps565(r,g,b); 
			pus_out+=height_y+Extra;
		} 
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 
*/
void yuv2rgb_565RZ(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither,int Brightness,uint8_t *puc_yp,uint8_t *puc_up,uint8_t *puc_vp, int slowopt, int invert) 
{ 

	register int x,y; 
	register signed int temp; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	if (nExtra<0) nExtra=0;
	if ((width_y<<1)>VisibleHeight) width_y=VisibleHeight>>1;
	if ((height_y<<1)>VisibleWidth) height_y=VisibleWidth>>1;
	pus_out = (unsigned short *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		pus_out = (unsigned short *) puc_out;
		pus_out+=y<<1;
		for (x=width_y-1;x!=0;x--)
		{ 
			temp=puc_y[x]-144+Brightness; 
			r = _S(temp+puc_u[x>>1]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x>>1]); 
			pus_out[0] = pus_out[1]= pus_out[(height_y+nExtra)] = pus_out[((height_y+nExtra))+1]=(unsigned short) _Ps565(r,g,b); 
			pus_out+=((height_y+nExtra)<<1);

		} 
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 

#else

void yuv2rgb_565R(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int Extra, int Dither )
{ 

	register int x,y; 
	register signed int temp; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	if (Extra<0) Extra=0;
	if (width_y>VisibleHeight)
	{
		puc_y+=(width_y-VisibleHeight)>>1;
		puc_u+=(width_y-VisibleHeight)>>2;
		puc_v+=(width_y-VisibleHeight)>>2;
		width_y=VisibleHeight;
	}
	if (height_y>VisibleWidth)
	{
		puc_y+=(height_y-VisibleWidth)*(stride_y>>1);
		puc_u+=(height_y-VisibleWidth)*(stride_uv>>2);
		puc_v+=(height_y-VisibleWidth)*(stride_uv>>2);
		height_y=VisibleWidth;
	}
	pus_out = (unsigned short *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		pus_out = (unsigned short *) puc_out;
		pus_out+=y;
		for (x=width_y-1;x!=0;x--)
		{ 
			temp=puc_y[x]-144; 
			r = _S(temp+puc_u[x>>1]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x>>1]); 
			pus_out[0] = (unsigned short) _Ps565(r,g,b); 
			pus_out+=height_y+Extra;
		} 
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 


void yuv2rgb_565RZ(uint8_t *puc_y, int stride_y, 
uint8_t *puc_u, uint8_t *puc_v, int stride_uv, 
uint8_t *puc_out, int width_y, int height_y, int nExtra, int Dither) 
{ 

	register int x,y; 
	register signed int temp; 
	unsigned short *pus_out; 
	register unsigned int r,g,b; 
	if (nExtra<0) nExtra=0;
	if ((width_y<<1)>VisibleHeight) width_y=VisibleHeight>>1;
	if ((height_y<<1)>VisibleWidth) height_y=VisibleWidth>>1;
	pus_out = (unsigned short *) puc_out; 
	for (y=0; y<height_y; y++) 
	{ 
		pus_out = (unsigned short *) puc_out;
		pus_out+=y<<1;
		for (x=width_y-1;x!=0;x--)
		{ 
			temp=puc_y[x]-144; 
			r = _S(temp+puc_u[x>>1]); 
			g = _S(temp+128); 
			b = _S(temp+puc_v[x>>1]); 
			pus_out[0] = pus_out[1]= pus_out[(height_y+nExtra)] = pus_out[((height_y+nExtra))+1]=(unsigned short) _Ps565(r,g,b); 
			pus_out+=((height_y+nExtra)<<1);

		} 
		puc_y += stride_y; 
		if (y<<31) 
		{ 
			puc_u += stride_uv; 
			puc_v += stride_uv; 
		} 
	} 
} 


#endif
#endif
