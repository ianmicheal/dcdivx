/*
 * H263/MPEG4 backend for ffmpeg encoder and decoder
 * Copyright (c) 2000,2001 Gerard Lantau.
 * H263+ support for custom picture format.
 * Copyright (c) 2001 Juan J. Sierralta P.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "avcodec.h"
#include "mpegvideo.h"
#include "h263data.h"
#include "mpeg4data.h"
#define NDEBUG


void mpeg4_pred_ac(MpegEncContext * s, INT16 *block, int n, 
                   int dir)
{
    int x, y, wrap, i;
    INT16 *ac_val, *ac_val1;

    /* find prediction */
    if (n < 4) {
	x = 2 * s->mb_x + 1 + (n & 1);
	y = 2 * s->mb_y + 1 + ((n & 2) >> 1);
	wrap = s->mb_width * 2 + 2;
	ac_val = s->ac_val[0][0];
    } else {
	x = s->mb_x + 1;
	y = s->mb_y + 1;
	wrap = s->mb_width + 2;
	ac_val = s->ac_val[n - 4 + 1][0];
    }
    ac_val += ((y) * wrap + (x)) * 16;
    ac_val1 = ac_val;
    if (s->ac_pred) {
        if (dir == 0) {
            /* left prediction */
            ac_val -= 16;
            for(i=1;i<8;i++) {
                block[i*8] += ac_val[i];
            }
        } else {
            /* top prediction */
            ac_val -= 16 * wrap;
            for(i=1;i<8;i++) {
                block[i] += ac_val[i + 8];
            }
        }
    }
    /* left copy */
    for(i=1;i<8;i++)
        ac_val1[i] = block[i * 8];
    /* top copy */
    for(i=1;i<8;i++)
        ac_val1[8 + i] = block[i];
}



static int mid_pred(int a, int b, int c)
{
    int vmin, vmax;
    vmin = a;
    if (b < vmin)
        vmin = b;
    if (c < vmin)
        vmin = c;

    vmax = a;
    if (b > vmax)
        vmax = b;
    if (c > vmax)
        vmax = c;

    return a + b + c - vmin - vmax;
}

INT16 *h263_pred_motion(MpegEncContext * s, int block, 
                        int *px, int *py)
{
    int x, y, wrap;
    INT16 *A, *B, *C, *mot_val;

    x = 2 * s->mb_x + 1 + (block & 1);
    y = 2 * s->mb_y + 1 + ((block >> 1) & 1);
    wrap = 2 * s->mb_width + 2;

    mot_val = s->motion_val[(x) + (y) * wrap];

    /* special case for first line */
    if (y == 1) {
        A = s->motion_val[(x-1) + (y) * wrap];
        *px = A[0];
        *py = A[1];
    } else {
        switch(block) {
        default:
        case 0:
            A = s->motion_val[(x-1) + (y) * wrap];
            B = s->motion_val[(x) + (y-1) * wrap];
            C = s->motion_val[(x+2) + (y-1) * wrap];
            break;
        case 1:
        case 2:
            A = s->motion_val[(x-1) + (y) * wrap];
            B = s->motion_val[(x) + (y-1) * wrap];
            C = s->motion_val[(x+1) + (y-1) * wrap];
            break;
        case 3:
            A = s->motion_val[(x-1) + (y) * wrap];
            B = s->motion_val[(x-1) + (y-1) * wrap];
            C = s->motion_val[(x) + (y-1) * wrap];
            break;
        }
        *px = mid_pred(A[0], B[0], C[0]);
        *py = mid_pred(A[1], B[1], C[1]);
    }
    return mot_val;
}





/***********************************************/
/* decoding */

static VLC intra_MCBPC_vlc;
static VLC inter_MCBPC_vlc;
static VLC cbpy_vlc;
static VLC mv_vlc;
static VLC dc_lum, dc_chrom;

void init_rl(RLTable *rl)
{
    INT8 max_level[MAX_RUN+1], max_run[MAX_LEVEL+1];
    UINT8 index_run[MAX_RUN+1];
    int last, run, level, start, end, i;

    /* compute max_level[], max_run[] and index_run[] */
    for(last=0;last<2;last++) {
        if (last == 0) {
            start = 0;
            end = rl->last;
        } else {
            start = rl->last;
            end = rl->n;
        }

        memset(max_level, 0, MAX_RUN + 1);
        memset(max_run, 0, MAX_LEVEL + 1);
        memset(index_run, rl->n, MAX_RUN + 1);
        for(i=start;i<end;i++) {
            run = rl->table_run[i];
            level = rl->table_level[i];
            if (index_run[run] == rl->n)
                index_run[run] = i;
            if (level > max_level[run])
                max_level[run] = level;
            if (run > max_run[level])
                max_run[level] = run;
        }
        rl->max_level[last] = malloc(MAX_RUN + 1);
        memcpy(rl->max_level[last], max_level, MAX_RUN + 1);
        rl->max_run[last] = malloc(MAX_LEVEL + 1);
        memcpy(rl->max_run[last], max_run, MAX_LEVEL + 1);
        rl->index_run[last] = malloc(MAX_RUN + 1);
        memcpy(rl->index_run[last], index_run, MAX_RUN + 1);
    }
}

void init_vlc_rl(RLTable *rl)
{
    init_vlc(&rl->vlc, 9, rl->n + 1, 
             &rl->table_vlc[0][1], 4, 2,
             &rl->table_vlc[0][0], 4, 2);
}

/* init vlcs */

