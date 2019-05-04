// msmpeg.cpp : Defines the entry point for the DLL application.
//

#include "msmpeg.h"
#include "avcodec.h"
#include "kos.h"
#include "decore.h"

AVCodec *codec;
AVCodecContext codec_context, *c = &codec_context;
int frame, got_picture, len;
//FILE *f;
AVPicture picture;


/**
 *
**/

extern unsigned char *decore_stream;
extern int decore_length;

int decore_init_Div3(int hor_size, int ver_size, unsigned long color_depth, int output_format);
int decore_release_Div3();
void decore_frame_Div3(unsigned char *stream, int length, unsigned char *bmp, int render_flag, int Extra);

/***/

static int flag_firstpicture = 1;

/***/

int decore_Div3(unsigned long handle, unsigned long dec_opt,
	void *param1, void *param2)
{
	switch (dec_opt)
	{
		case DEC_OPT_INIT:
		{
			DEC_PARAM *dec_param = (DEC_PARAM *) param1;
 			int x_size = dec_param->x_dim;
 			int y_size = dec_param->y_dim;
			unsigned long color_depth = dec_param->color_depth;
			int output_format = dec_param->output_format;

			divxinit(x_size,y_size);

			return DEC_OK;
		}
		break; 
		case DEC_OPT_RELEASE:
		{
			divxdeinit();
			return DEC_OK;
		}
		break;
		default:
		{

			return DEC_OK;
		}
		break;
	}
}

/***/

/***/

void decore_frame_Div3(unsigned char *stream, int length, unsigned char *bmp, int render_flag, int Extra)
{
    avcodec_decode_video(c, &picture, &got_picture, 
                               stream, 48000,bmp,render_flag,Extra);

}



void divxinit(int x, int y)
{

	register_all();
    /* find the mpeg1 video decoder */
    codec = avcodec_find_decoder(CODEC_ID_MSMPEG4);
    if (!codec) {
//        fprintf(stderr, "codec not found\n");
        //exit(1);
    }

    /* put default values */
    memset(c, 0, sizeof(*c));
	c->width=x;
	c->height=y;
    /* open it */
    if (avcodec_open(c, codec) < 0) {
//        fprintf(stderr, "could not open codec\n");
//        exit(1);
    }
    
}


void divxdeinit()
{
    avcodec_close(c);
}


