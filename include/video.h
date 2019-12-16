#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#ifndef _VIDEO_H
#define _VIDEO_H

#define VIDEO_DISPLAY_BUFFER_NUM 2 
#define VIDEO_FLIP_RATE 0 
#define VIDEO_FLIP_MODE_VSYNC 1
#define VIDEO_FLIP_MODE_HSYNC 2
#define VIDEO_FLIP_MODE_WINDOW 3
#define VIDEO_MODE_TILE 0
#define VIDEO_MODE_LINEAR 1

#define ATTR_WIDTH 1280 
#define ATTR_HEIGHT 720

typedef struct SceVideoOutBufferAttribute {
	int32_t format;
	int32_t tmode;
	int32_t aspect;
	uint32_t width;
	uint32_t height;
	uint32_t pixelPitch;
	uint64_t reserved[2];
} SceVideoOutBufferAttribute;

typedef struct SceVideoOutFlipStatus {
	uint64_t num;
	uint64_t ptime;
	uint64_t stime;
	int64_t flipArg;
	uint64_t reserved[2];
	int32_t numGpuFlipPending;
	int32_t numFlipPending;
	int32_t currentBuffer;
	uint32_t reserved1;
} SceVideoOutFlipStatus;

int videoFinish();
int videoInit();
int videoInitHandle();
int videoInitMemory();
void videoAllocDisplayBuffer(int displayBufNum);
int videoInitDisplayBuffer(int num, int bufIndexStart);
void videoClearBuffer(char flag);
void videoSwapBuffers();
void *videoMalloc(int size);
int videoWaitFlipArg(SceKernelEqueue *flipQueue);
void videoFinishDrawing(int64_t flipArg);
void videoStartDrawing();
void videoDrawPixelColor(int x, int y, uint32_t pixelColor);
uint32_t videoGetRGB(int r, int g, int b);
void videoDebugSetFontColor(uint32_t color);
void videoDebugSetBackFontColor(uint32_t color);
void videoDebugDrawCharacter(int character, int x, int y, int scale);
int videoDebugSizeStringWidth(int scale, const char *str);
int videoDebugSizeStringHeight(int scale, const char *str);
void videoDebugDrawString(int x, int y, int scale, const char *str);

#endif