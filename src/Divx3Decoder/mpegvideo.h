/*
 * Generic DCT based hybrid video encoder
 * Copyright (c) 2000,2001 Gerard Lantau.
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
#ifndef __MPEGVID
#define __MPEGVID

#include "common.h"
/* Macros for picture code type. */
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3

enum OutputFormat {
    FMT_MPEG1,
    FMT_H263,
    FMT_MJPEG,
};

#define MPEG_BUF_SIZE (16 * 1024)

typedef struct MpegEncContext {
    /* the following parameters must be initialized before encoding */
    int width, height; /* picture size. must be a multiple of 16 */
    int gop_size;
    int frame_rate; /* number of frames per second */
    int intra_only; /* if true, only intra pictures are generated */
    int bit_rate;        /* wanted bit rate */
    enum OutputFormat out_format; /* output format */
    int h263_plus; /* h263 plus headers */
    int h263_rv10; /* use RV10 variation for H263 */
    int h263_pred; /* use OpenDIVX (aka mpeg4) ac/dc predictions */
    int h263_msmpeg4; /* generate MSMPEG4 compatible stream */
    int fixed_qscale; /* fixed qscale if non zero */
    /* the following fields are managed internally by the encoder */

    /* bit output */
    PutBitContext pb;

    /* sequence parameters */
    int context_initialized;
    int picture_number;
    int fake_picture_number; /* picture number at the bitstream frame rate */
    int gop_picture_number; /* index of the first picture of a GOP */
    int mb_width, mb_height;
    int linesize;              /* line size, in bytes, may be different from width */
    UINT8 *new_picture[3];     /* picture to be compressed */
    UINT8 *last_picture[3];    /* previous picture */
    UINT8 *last_picture_base[3]; /* real start of the picture */
    UINT8 *next_picture[3];    /* previous picture (for bidir pred) */
    UINT8 *next_picture_base[3]; /* real start of the picture */
    UINT8 *aux_picture[3];    /* aux picture (for B frames only) */
    UINT8 *aux_picture_base[3]; /* real start of the picture */
    UINT8 *current_picture[3]; /* buffer to store the decompressed current picture */
    int last_dc[3]; /* last DC values for MPEG1 */
    INT16 *dc_val[3]; /* used for mpeg4 DC prediction */
    int y_dc_scale, c_dc_scale;
    UINT8 *coded_block; /* used for coded block pattern prediction */
    INT16 (*ac_val[3])[16]; /* used for for mpeg4 AC prediction */
    int ac_pred;

    int qscale;
    int pict_type;
    int frame_rate_index;
    /* motion compensation */
    int unrestricted_mv;
    int f_code; /* resolution */
    INT16 (*motion_val)[2]; /* used for MV prediction */
    int full_search;
    int mv_dir;
#define MV_DIR_BACKWARD  1
#define MV_DIR_FORWARD   2
    int mv_type;
#define MV_TYPE_16X16       0   /* 1 vector for the whole mb */
#define MV_TYPE_8X8         1   /* 4 vectors (h263) */
#define MV_TYPE_16X8        2   /* 2 vectors, one per 16x8 block */ 
#define MV_TYPE_FIELD       3   /* 2 vectors, one per field */ 
#define MV_TYPE_DMV         4   /* 2 vectors, special mpeg2 Dual Prime Vectors */
    /* motion vectors for a macroblock 
       first coordinate : 0 = forward 1 = backward
       second "         : depend on type
       third  "         : 0 = x, 1 = y
    */
    int mv[2][4][2];
    int field_select[2][2];
    int last_mv[2][2][2];

    int has_b_frames;
    int no_rounding; /* apply no rounding to motion estimation (MPEG4) */

    /* macroblock layer */
    int mb_x, mb_y;
    int mb_incr;
    int mb_intra;
    /* matrix transmitted in the bitstream */
    UINT16 intra_matrix[64];
    UINT16 chroma_intra_matrix[64];
    UINT16 non_intra_matrix[64];
    UINT16 chroma_non_intra_matrix[64];
    /* precomputed matrix (combine qscale and DCT renorm) */
    int q_intra_matrix[64];
    int q_non_intra_matrix[64];
    int block_last_index[6];  /* last non zero coefficient in block */

    void *opaque; /* private data for the user */

    /* bit rate control */
    int I_frame_bits;    /* wanted number of bits per I frame */
    int P_frame_bits;    /* same for P frame */
    long wanted_bits;
    long total_bits;
    /* MJPEG specific */
    struct MJpegContext *mjpeg_ctx;
    /* MSMPEG4 specific */
    int mv_table_index;
    int rl_table_index;
    int rl_chroma_table_index;
    int dc_table_index;
    int use_skip_mb_code;
    /* decompression specific */
    GetBitContext gb;

    /* MPEG2 specific - I wish I had not to support this mess. */
    int progressive_sequence;
    int mpeg_f_code[2][2];
    int picture_structure;
/* picture type */
#define PICT_TOP_FIELD     1
#define PICT_BOTTOM_FIELD  2
#define PICT_FRAME         3

    int intra_dc_precision;
    int frame_pred_frame_dct;
    int top_field_first;
    int concealment_motion_vectors;
    int q_scale_type;
    int intra_vlc_format;
    int alternate_scan;
    int repeat_first_field;
    int chroma_420_type;
    int progressive_frame;
    int mpeg2;
    int full_pel[2];
    int interlaced_dct;
    int last_qscale;
    int first_slice;
} MpegEncContext;


int MPV_common_init(MpegEncContext *s);
void MPV_common_end(MpegEncContext *s);
void MPV_decode_mb(MpegEncContext *s, DCTELEM block[6][64]);
void MPV_frame_start(MpegEncContext *s);
void MPV_frame_end(MpegEncContext *s);

/* motion_est.c */

int estimate_motion(MpegEncContext *s, 
                    int mb_x, int mb_y,
                    int *mx_ptr, int *my_ptr);

/* mpeg12.c */
extern const UINT8 default_intra_matrix[64];
extern const UINT8 default_non_intra_matrix[64];

void mpeg1_encode_picture_header(MpegEncContext *s, int picture_number);
void mpeg1_encode_mb(MpegEncContext *s,
                     DCTELEM block[6][64],
                     int motion_x, int motion_y);

/* h263enc.c */

/* run length table */
#define MAX_RUN    64
#define MAX_LEVEL  64

typedef struct RLTable {
    int n; /* number of entries of table_vlc minus 1 */
    int last; /* number of values for last = 0 */
    const UINT16 (*table_vlc)[2];
    const INT8 *table_run;
    const INT8 *table_level;
    UINT8 *index_run[2]; /* encoding only */
    INT8 *max_level[2]; /* encoding & decoding */
    INT8 *max_run[2];   /* encoding & decoding */
    VLC vlc;            /* decoding only */
} RLTable;

void init_rl(RLTable *rl);
void init_vlc_rl(RLTable *rl);

void h263_encode_mb(MpegEncContext *s, 
                    DCTELEM block[6][64],
                    int motion_x, int motion_y);
void h263_encode_picture_header(MpegEncContext *s, int picture_number);
void rv10_encode_picture_header(MpegEncContext *s, int picture_number);
void h263_dc_scale(MpegEncContext *s);
INT16 *h263_pred_motion(MpegEncContext * s, int block, 
                        int *px, int *py);
void mpeg4_pred_ac(MpegEncContext * s, INT16 *block, int n, 
                   int dir);
void mpeg4_encode_picture_header(MpegEncContext *s, int picture_number);
void h263_encode_init_vlc(MpegEncContext *s);

void h263_decode_init_vlc(MpegEncContext *s);
int h263_decode_picture_header(MpegEncContext *s);
int mpeg4_decode_picture_header(MpegEncContext * s);
int h263_decode_mb(MpegEncContext *s,
                   DCTELEM block[6][64]);
int h263_get_picture_format(int width, int height);
extern UINT8 ff_alternate_horizontal_scan[64];
extern UINT8 ff_alternate_vertical_scan[64];

/* msmpeg4.c */
void msmpeg4_encode_picture_header(MpegEncContext * s, int picture_number);
void msmpeg4_encode_mb(MpegEncContext * s, 
                       DCTELEM block[6][64],
                       int motion_x, int motion_y);
void msmpeg4_dc_scale(MpegEncContext * s);
int msmpeg4_decode_picture_header(MpegEncContext * s);
int msmpeg4_decode_mb(MpegEncContext *s, 
                      DCTELEM block[6][64]);
int msmpeg4_decode_init_vlc(MpegEncContext *s);

/* mjpegenc.c */

int mjpeg_init(MpegEncContext *s);
void mjpeg_close(MpegEncContext *s);
void mjpeg_encode_mb(MpegEncContext *s, 
                     DCTELEM block[6][64]);
void mjpeg_picture_header(MpegEncContext *s);
void mjpeg_picture_trailer(MpegEncContext *s);

#endif