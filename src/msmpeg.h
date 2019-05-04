
// decore options
#define DEC_OPT_INIT		0x00008000
#define DEC_OPT_RELEASE 0x00010000
#define DEC_OPT_SETPP		0x00020000 // set postprocessing mode
#define DEC_OPT_SETOUT  0x00040000 // set output mode

// decore return values
#define DEC_OK					0
#define DEC_MEMORY			1
#define DEC_BAD_FORMAT	2

// supported output formats
#define YUV12		1
#define RGB32		2
#define RGB24		3
#define RGB555	4
#define RGB565  5
#define YUV2    6
#define RGB565R  7
#define RGB565Z  8
#define RGB565RZ  9
#define RGB565ZPP  10

/**
 *
**/
/*
typedef struct _DEC_PARAM_ 
{
	int x_dim; // x dimension of the frames to be decoded
	int y_dim; // y dimension of the frames to be decoded
	unsigned long color_depth; // leaved for compatibility (new value must be NULL)
	int output_format;
	int dither;
} DEC_PARAM;

typedef struct _DEC_FRAME_
{
	void *bmp; // the 24-bit decoded bitmap 
	void *bitstream; // the decoder buffer
	long length; // the lenght of the decoder stream
	int render_flag;
} DEC_FRAME;

typedef struct _DEC_SET_
{
	int postproc_level; // valid interval are [0..100]
} DEC_SET;
*/
/**
 *
**/
// the prototype of the decore() - main decore engine entrance
//
int decore_frame_Div3(unsigned char *stream, int length, unsigned char *bmp,int flag, int Extra);
//int decore_dropframe(unsigned char *stream, int length, unsigned char *bmp);

int decore_Div3(
			unsigned long handle,	// handle	- the handle of the calling entity, must be unique
			unsigned long dec_opt, // dec_opt - the option for docoding, see below
			void *param1,	// param1	- the parameter 1 (it's actually meaning depends on dec_opt
			void *param2);	// param2	- the parameter 2 (it's actually meaning depends on dec_opt




 void divxinit(int width, int height);
 void divxdecode(unsigned char* inbuf_ptr, int size, unsigned char* rgbbuff, int show, int rgbstep);

 void divxdeinit();
