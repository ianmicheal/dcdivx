/*
 * H263 decoder
 * Copyright (c) 2001 Gerard Lantau.
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
#include "avcodec.h"
#include "mpegvideo.h"
//#include "msmpeg4data.h"
//#define DEBUG

static int h263_decode_init(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;
    
    s->out_format = FMT_H263;

    s->width = avctx->width;
    s->height = avctx->height;

    /* select sub codec */
    s->h263_msmpeg4 = 1;
    s->h263_pred = 1;

    /* for h263, we allocate the images after having read the header */
    if (MPV_common_init(s) < 0)
        return -1;

    msmpeg4_decode_init_vlc(s);
    
    return 0;
}

static int h263_decode_end(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;

    MPV_common_end(s);
    return 0;
}

static int h263_decode_frame(AVCodecContext *avctx, 
                             void *data, int *data_size,
                             UINT8 *buf, int buf_size)
{
    MpegEncContext *s = avctx->priv_data;
    int ret;
    DCTELEM block[6][64];
    AVPicture *pict = data; 


    /* no supplementary picture */
    if (buf_size == 0) {
        *data_size = 0;
        return 0;
    }

    init_get_bits(&s->gb, buf, buf_size);

    ret = msmpeg4_decode_picture_header(s);
    if (ret < 0)
        return -1;

    MPV_frame_start(s);


    /* decode each macroblock */
    for(s->mb_y=0; s->mb_y < s->mb_height; s->mb_y++) {
        for(s->mb_x=0; s->mb_x < s->mb_width; s->mb_x++) {
            /* DCT & quantize */
            msmpeg4_dc_scale(s);

            memset(block, 0, sizeof(block));
            s->mv_dir = MV_DIR_FORWARD;
            s->mv_type = MV_TYPE_16X16; 
                if (msmpeg4_decode_mb(s, block) < 0)
                    return -1;
            MPV_decode_mb(s, block);
        }
    }

    MPV_frame_end(s);
    
    pict->data[0] = s->current_picture[0];
    pict->data[1] = s->current_picture[1];
    pict->data[2] = s->current_picture[2];
    pict->linesize[0] = s->linesize;
    pict->linesize[1] = s->linesize / 2;
    pict->linesize[2] = s->linesize / 2;
	pict->last_picture[0]=s->last_picture[0];
	pict->last_picture[1]=s->last_picture[1];
	pict->last_picture[2]=s->last_picture[2];
    avctx->quality = s->qscale;
    *data_size = sizeof(AVPicture);
    return buf_size;
}

AVCodec msmpeg4_decoder = {
    "msmpeg4",
    CODEC_TYPE_VIDEO,
    CODEC_ID_MSMPEG4,
    sizeof(MpegEncContext),
    h263_decode_init,
    NULL,
    h263_decode_end,
    h263_decode_frame,
};
