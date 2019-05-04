/*
 * utils for libavcodec
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

#include "common.h"
#include "dsputil.h"
#include "avcodec.h"
//#include "../yuv2rgb.h"



/* memory alloc */
void *av_mallocz(int size)
{
    void *ptr;
    ptr = malloc(size);
    if (!ptr)
        return NULL;
    memset(ptr, 0, size);
    return ptr;
}

/* encoder management */
AVCodec *first_avcodec;
void register_avcodec(AVCodec *format)
{
    AVCodec **p;
    p = &first_avcodec;
    while (*p != NULL) p = &(*p)->next;
    *p = format;
	if (!first_avcodec) first_avcodec=format;
    format->next = NULL;
}

void avcodec_register_all(void)
{
    register_avcodec(&msmpeg4_decoder);
}

int avcodec_open(AVCodecContext *avctx, AVCodec *codec)
{
    int ret;
    avctx->codec = codec;
    avctx->frame_number = 0;

    avctx->priv_data = av_mallocz(codec->priv_data_size);
    if (!avctx->priv_data) 
        return -27;
    ret = avctx->codec->init(avctx);
    if (ret < 0) {
        free(avctx->priv_data);
        avctx->priv_data = NULL;
        return ret;
    }
    return 0;
}

int avcodec_encode_audio(AVCodecContext *avctx, UINT8 *buf, int buf_size, 
                         const short *samples)
{
    int ret;

    ret = avctx->codec->encode(avctx, buf, buf_size, (void *)samples);
    avctx->frame_number++;
    return ret;
}

int avcodec_encode_video(AVCodecContext *avctx, UINT8 *buf, int buf_size, 
                         const AVPicture *pict)
{
    int ret;

    ret = avctx->codec->encode(avctx, buf, buf_size, (void *)pict);
    avctx->frame_number++;
    return ret;
}

/* decode a frame. return -1 if error, otherwise return the number of
   bytes used. If no frame could be decompressed, *got_picture_ptr is
   zero. Otherwise, it is non zero */
int avcodec_decode_video(AVCodecContext *avctx, AVPicture *picture, 
                         int *got_picture_ptr,
                         UINT8 *buf, int buf_size, unsigned char* rgbbuff, int show, int rgbstep)
{
    int ret;
    ret = avctx->codec->decode(avctx, picture, got_picture_ptr, 
                               buf, buf_size);
	if (show) memcpy(rgbbuff,(char*)picture,*got_picture_ptr);
//	if (show) convert_yuv(picture->data[0],picture->linesize[0],picture->data[1],picture->data[2],picture->linesize[2],rgbbuff,avctx->width,avctx->height,rgbstep,0);
    avctx->frame_number++;
	
    return ret;
}

/* decode an audio frame. return -1 if error, otherwise return the
   *number of bytes used. If no frame could be decompressed,
   *frame_size_ptr is zero. Otherwise, it is the decompressed frame
   *size in BYTES. */
int avcodec_decode_audio(AVCodecContext *avctx, INT16 *samples, 
                         int *frame_size_ptr,
                         UINT8 *buf, int buf_size)
{
    int ret;

    ret = avctx->codec->decode(avctx, samples, frame_size_ptr, 
                               buf, buf_size);
    avctx->frame_number++;
    return ret;
}

int avcodec_close(AVCodecContext *avctx)
{
    if (avctx->codec->close)
        avctx->codec->close(avctx);
    free(avctx->priv_data);
    avctx->priv_data = NULL;
    avctx->codec = NULL;
    return 0;
}

AVCodec *avcodec_find_encoder(enum CodecID id)
{
    AVCodec *p;
    p = first_avcodec;
    while (p) {
        if (p->encode != NULL && p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

AVCodec *avcodec_find_decoder(enum CodecID id)
{
    AVCodec *p;
    p = first_avcodec;
    while (p) {
        if (p->decode != NULL && p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

AVCodec *avcodec_find(enum CodecID id)
{
    AVCodec *p;
    p = first_avcodec;
    while (p) {
        if (p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

void avcodec_string(char *buf, int buf_size, AVCodecContext *enc)
{
}

/* must be called before any other functions */
void avcodec_init(void)
{
    dsputil_init();
}


static int encode_init(AVCodecContext *s)
{
    return 0;
}

static int decode_frame(AVCodecContext *avctx, 
                        void *data, int *data_size,
                        UINT8 *buf, int buf_size)
{
    return -1;
}

static int encode_frame(AVCodecContext *avctx,
                        unsigned char *frame, int buf_size, void *data)
{
    return -1;
}
void register_all(void)
{
    avcodec_init();
    avcodec_register_all();

}


