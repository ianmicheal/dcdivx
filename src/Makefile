# Basic KallistiOS Makefile for Projects without ROMDISK

TARGET = divx.elf
OBJS = common.o utils.o h263dec.o mpegvideo.o h263.o dsputil.o msmpeg4.o motion_est.o jrevdct.o MP3Reader.o divx_player.o texture.o bkg.o 3dutils.o songmenu.o sndstream.o romdisk.o decore.o getbits.o video.o mp4_decoder.o mp4_header.o mp4_mblock.o mp4_picture.o mp4_predict.o mp4_recon.o mp4_vld.o yuv2rgb.o AviDecaps.o InputMedia.o bit.o fixed.o frame.o huffman.o layer3.o stream.o synth.o divx.o 

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)
	-rm -f romdisk.o romdisk.img

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS) 

	$(KOS_CC) $(KOS_CFLAGS) -O0 $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA) -L$(KOS_BASE)/lib -ldcutils -lkallisti -lm -lgcc

	$(KOS_OBJCOPY) -O binary $(TARGET) divx.bin

romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_CC_BASE)/bin/sh-elf-strip $(TARGET)

