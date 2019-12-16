// Based on Orbis2D from liborbis
// https://github.com/orbisdev/liborbis/blob/master/liborbis2d/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#include "font.h"
#include "video.h"

int gnm_id;
int video_id;
int pngdec_id;

int video_handle = -1;
SceKernelEqueue flipQueue;

static uint32_t *dumpBuf = NULL;
static int bufSize;

uint32_t videoMemStackSize;
uint64_t videoMemStackAddr;
uint64_t videoMemStackTopAddr;
uint64_t videoMemStackBaseAddr;
void* videoMemOffset;

uint8_t currentBuffer;
void  *surfaceAddr[VIDEO_DISPLAY_BUFFER_NUM];
int64_t flipArgLog[VIDEO_DISPLAY_BUFFER_NUM];

// Using Orignal SDK will crash the code ?! + Gnm doesn't exist xP
void (*sceGnmFlushGarlic)(void) = NULL;
int  (*sceVideoOutOpen)(SceUserServiceUserId userId, int type, int index, const void *param) = NULL;
int  (*sceVideoOutClose)(int handle) = NULL;
int  (*sceVideoOutRegisterBuffers)(int handle, int initialIndex, void * const *addr, int numBuf, const SceVideoOutBufferAttribute *attr) = NULL;
int  (*sceVideoOutUnregisterBuffers)(int handle, int indexAttr) = NULL;
int  (*sceVideoOutSubmitFlip)(int handle, int indexBuf, unsigned int flipMode, int64_t flipArg) = NULL;
void (*sceVideoOutSetBufferAttribute)(SceVideoOutBufferAttribute *attr, unsigned int format, unsigned int tmode,unsigned int aspect, unsigned int width, unsigned int height, unsigned int pixelPitch) = NULL;
int  (*sceVideoOutSetFlipRate)(int handle, int flipRate) = NULL;
int  (*sceVideoOutAddFlipEvent)(SceKernelEqueue eq, int handle, void *data) = NULL;
int  (*sceVideoOutGetFlipStatus)(int handle, SceVideoOutFlipStatus *status) = NULL;

uint32_t font_color=0x80FFFFFF;
uint32_t backfont_color=0x80000000;

//int fd = -1;
void write_log(char* str, ...) {
    va_list ap;
    va_start(ap, str);
	char buffer[2000] = {0};
	vsnprintf(buffer, 2000, str, ap);
	printf("%s", buffer); // Print data anyway

	/*
	if (fd <= 0)
		fd = sceKernelOpen("/data/log.txt", 0x0001 | 0x0200, 0777);

	if (fd > 0) {
		sceKernelWrite(fd, buffer, strlen(buffer));
	}
	*/

    va_end(ap);
}

int videoFinish()
{
	write_log("finish video out\n");
	int ret = sceVideoOutClose(video_handle);
	if(dumpBuf) free(dumpBuf), dumpBuf = NULL;

	sceKernelStopUnloadModule(pngdec_id, 0, NULL, 0, NULL, NULL);
	sceKernelStopUnloadModule(gnm_id, 0, NULL, 0, NULL, NULL);
	sceKernelStopUnloadModule(video_id, 0, NULL, 0, NULL, NULL);
	return ret;
}

int videoInit()
{
	write_log("video init\n");

	int bufIndex;

	gnm_id = sceKernelLoadStartModule("libSceGnmDriver.sprx", 0, NULL, 0, NULL, NULL);
	video_id = sceKernelLoadStartModule("libSceVideoOut.sprx", 0, NULL, 0, NULL, NULL);
	pngdec_id = sceKernelLoadStartModule("libScePngDec.sprx", 0, NULL, 0, NULL, NULL);

	sceKernelDlsym(gnm_id,   "sceGnmFlushGarlic", &sceGnmFlushGarlic);
	sceKernelDlsym(video_id, "sceVideoOutOpen", &sceVideoOutOpen);
	sceKernelDlsym(video_id, "sceVideoOutClose", &sceVideoOutClose);
	sceKernelDlsym(video_id, "sceVideoOutRegisterBuffers", &sceVideoOutRegisterBuffers);
	sceKernelDlsym(video_id, "sceVideoOutUnregisterBuffers", &sceVideoOutUnregisterBuffers);
	sceKernelDlsym(video_id, "sceVideoOutSubmitFlip", &sceVideoOutSubmitFlip);
	sceKernelDlsym(video_id, "sceVideoOutSetBufferAttribute", &sceVideoOutSetBufferAttribute);
	sceKernelDlsym(video_id, "sceVideoOutSetFlipRate", &sceVideoOutSetFlipRate);
	sceKernelDlsym(video_id, "sceVideoOutAddFlipEvent", &sceVideoOutAddFlipEvent);
	sceKernelDlsym(video_id, "sceVideoOutGetFlipStatus", &sceVideoOutGetFlipStatus);
	/*
	sceKernelDlsym(pngdec_id, "scePngDecQueryMemorySize", &scePngDecQueryMemorySize);
	sceKernelDlsym(pngdec_id, "scePngDecCreate", &scePngDecCreate);
	sceKernelDlsym(pngdec_id, "scePngDecDelete", &scePngDecDelete);
	*/
	
	video_handle = videoInitHandle();
	write_log("Video Handle result: %i\n", video_handle);

	if (video_handle < 0)
		return -1;

	int ret = 0;
	ret = videoInitMemory();
	write_log("Video Init Memory result: %i (0x%08x)\n", ret, ret);

	videoAllocDisplayBuffer(VIDEO_DISPLAY_BUFFER_NUM);
	videoInitDisplayBuffer(VIDEO_DISPLAY_BUFFER_NUM, 0);

	// set status of each buffer with flipArg
	for(bufIndex=0;bufIndex<VIDEO_DISPLAY_BUFFER_NUM;bufIndex++) 
	{
		flipArgLog[bufIndex]= -2; 
	}

	// prepare initial clear color to the display buffers
	for (bufIndex=0;bufIndex<VIDEO_DISPLAY_BUFFER_NUM;bufIndex++) 
	{
		videoClearBuffer(1);
		videoSwapBuffers();
	}

	write_log("sceVideoOutSetFlipRate calling ...\n");
	ret = sceVideoOutSetFlipRate(video_handle, VIDEO_FLIP_RATE);
	write_log("sceVideoOutSetFlipRate: %i (0x%08x)\n", ret, ret);

	return video_handle;
}

int videoInitHandle()
{
	write_log("videoInitHandle called.\n");

	int ret;
	write_log("sceVideoOutOpen calling ...\n");
	int video = sceVideoOutOpen(0xFF, 0, 0, NULL);
	write_log("sceVideoOutOpen : %i (0x%08x)\n", video, video);

	write_log("sceKernelCreateEqueue calling ...\n");
	ret = sceKernelCreateEqueue(&flipQueue, "queue to wait flip");
	write_log("sceKernelCreateEqueue = %i (0x%08x)\n", ret, ret);
	write_log("sceVideoOutAddFlipEvent calling ...\n");
	ret = sceVideoOutAddFlipEvent(flipQueue, video, NULL);
	write_log("sceVideoOutAddFlipEvent = %i (0x%08x)\n", ret, ret);

	return video;
}

int videoInitMemory()
{

	int ret;
	void* start;

	const uint32_t align=2*1024*1024;
	videoMemStackSize = 1024*1024* 192;

	write_log("sceKernelAllocateDirectMemory calling ...\n");
	ret = sceKernelAllocateDirectMemory(0,sceKernelGetDirectMemorySize(),videoMemStackSize,align,3, &start);
	write_log("sceKernelAllocateDirectMemory = %i (0x%08x)\n", ret, ret);

	if(ret==0)
	{

		videoMemOffset = start;

		void* pointer=NULL;
		
		write_log("sceKernelMapDirectMemory calling ...\n");
		ret = sceKernelMapDirectMemory(&pointer,videoMemStackSize,0x33,0,start,align);
		write_log("sceKernelMapDirectMemory = %i (0x%08x)\n", ret, ret);
		if(ret==0)
		{
			videoMemStackBaseAddr = (uintptr_t)pointer;
			videoMemStackTopAddr= videoMemStackBaseAddr + videoMemStackSize;
			videoMemStackAddr= videoMemStackBaseAddr;
		} else {
			write_log("sceKernelMapDirectMemory error : %i (0x%08x)\n", ret, ret);
		}
	} else {
		write_log("sceKernelAllocateDirectMemory error : %i (0x%08x)\n", ret, ret);
	}

	return ret;
}

void videoAllocDisplayBuffer(int displayBufNum)
{
	// cache the framebuffer size once, then reuse value
	bufSize = ATTR_WIDTH*ATTR_HEIGHT*4;

	for(int i=0;i<displayBufNum;i++)
	{
		surfaceAddr[i]= videoMalloc(bufSize);
	}
}

int videoInitDisplayBuffer(int num, int bufIndexStart)
{
	SceVideoOutBufferAttribute attr;
	write_log("sceVideoOutSetBufferAttribute calling ...\n");
	sceVideoOutSetBufferAttribute(&attr, 0x80000000, VIDEO_MODE_LINEAR, 0, ATTR_WIDTH, ATTR_HEIGHT, ATTR_WIDTH);
	write_log("sceVideoOutRegisterBuffers calling ...\n");
	int ret = sceVideoOutRegisterBuffers(video_handle, bufIndexStart, surfaceAddr, num, &attr);
	write_log("sceVideoOutRegisterBuffers : %i (0x%08x)\n", ret, ret);

	return ret;
}

void videoClearBuffer(char flag)
{
	if(!flag
	&& dumpBuf)
	{
		memcpy(surfaceAddr[currentBuffer], dumpBuf, bufSize);
	}
	else
	{
		uint64_t *px = surfaceAddr[currentBuffer],
		           c = (unsigned long long) 0x80000000 << 32 | 0x80000000; // Black background

		for(int i=0; i<(bufSize/sizeof(uint64_t)); i++)
		{
			memcpy(px, &c, sizeof(uint64_t)); px++;
		}
	}
}

void videoSwapBuffers()
{
	currentBuffer = (currentBuffer+1)%VIDEO_DISPLAY_BUFFER_NUM;
}

void *videoMalloc(int size)
{
	uint64_t offset = videoMemStackAddr;

	if((videoMemStackAddr+size) > videoMemStackTopAddr)
	{		
		return NULL;
	}
	videoMemStackAddr += size;
	
	write_log("Allocating video memory: %p (Size: %i)\n", (void *)(offset), size);

	return (void *)(offset);
}

int videoWaitFlipArg(SceKernelEqueue *flipQueue)
{
	int ret;
	int event_out;
	SceKernelEvent event;
	SceVideoOutFlipStatus status;
	
	while(1)
	{
		ret=sceVideoOutGetFlipStatus(video_handle, &status);

		if(ret>=0)
		{
			if(status.flipArg>=(flipArgLog[currentBuffer] +1))
			{
				write_log("status.flipArg> >= ...\n");
				return 0;
			}

			ret = sceKernelWaitEqueue(*flipQueue, &event, 1, &event_out, 0);
		} else {
			write_log("sceVideoOutGetFlipStatus error : %i (0x%08x)\n", ret, ret);
		}
	}

	return ret;
}

void videoFinishDrawing(int64_t flipArg)
{
	int ret;

	sceGnmFlushGarlic();

	// request flip to the buffer
	ret = sceVideoOutSubmitFlip(video_handle, currentBuffer, VIDEO_FLIP_MODE_VSYNC, flipArg);
	flipArgLog[currentBuffer] = flipArg;

}
void videoStartDrawing()
{
	videoWaitFlipArg(&flipQueue);
}

void videoDrawPixelColor(int x, int y, uint32_t pixelColor)
{
	write_log("Drawing pixel ...");

	if((x > -1 && x < ATTR_WIDTH)
	&& (y > -1 && y < ATTR_HEIGHT))
	{
		int pixel = (y * ATTR_WIDTH) + x;
		((uint32_t *)surfaceAddr[currentBuffer])[pixel]=pixelColor;
	}

	write_log("Drawing pixel done.");

}

uint32_t videoGetRGB(int r, int g, int b) {
	r=r%256;
	g=g%256;
	b=b%256;
	return 0x80000000|r<<16|g<<8|b;
}

void videoDebugSetFontColor(uint32_t color) {
	font_color = color;
}

void videoDebugSetBackFontColor(uint32_t color) {
	backfont_color = color;
}

void videoDebugDrawCharacter(int character, int x, int y, int scale) {
    for (int yy = 0; yy < 10 ; yy++) {
        uint8_t charPos = font[character * 10 + yy];
        int off = 8;
        for (int xx = 0; xx < 8; xx++) {           // font color : background color
			uint32_t clr = ((charPos >> xx) & 1) ? font_color : backfont_color;  // 0x00000000

			for (int sy = 0; sy < scale; sy++) {
				for (int sx = 0; sx < scale; sx++) {
					videoDrawPixelColor(x + (off * scale) - sx, y + (yy * scale) + sy, clr);
				}
			}

			off--;
        }
    }
}

int videoDebugSizeStringWidth(int scale, const char *str) {
	int max_len = 0;
	int current_len = 0;

    for (size_t i = 0; i < strlen(str); i++) {
    	if (str[i] == '\n') {
    		if (current_len > max_len) {
    			max_len = current_len;
    		}
    		current_len = 0;
    		continue;
    	}
    	current_len += 8 * scale;
    }

	if (current_len > max_len) {
		max_len = current_len;
	}
    		
	return max_len;
}

int videoDebugSizeStringHeight(int scale, const char *str) {
	int y_padding = scale * 10;

    for (size_t i = 0; i < strlen(str); i++) {
    	if (str[i] == '\n') {
    		y_padding += scale * 10;
    	}
    }

	return y_padding;
}


void videoDebugDrawString(int x, int y, int scale, const char *str) {
	int x_padding = 0;
	int y_padding = 0;

    for (size_t i = 0; i < strlen(str); i++) {
    	switch (str[i]) {
    		case '\n':
    			y_padding += scale * 10;
    			x_padding = 0;
    			continue;
    	}

    	videoDebugDrawCharacter(str[i], x + x_padding, y + y_padding, scale);
    	x_padding += 8 * scale;
    }
}
