// Based on OrbisPAD from liborbis
// https://github.com/psxdev/liborbis/blob/master/libPAD/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#ifndef _PAD_H
#define _PAD_H

#define	PAD_L3			0x00000002
#define	PAD_R3			0x00000004
#define	PAD_OPTIONS		0x00000008
#define	PAD_UP			0x00000010
#define	PAD_RIGHT		0x00000020
#define	PAD_DOWN		0x00000040
#define	PAD_LEFT		0x00000080
#define	PAD_L2			0x00000100
#define	PAD_R2			0x00000200
#define	PAD_L1			0x00000400
#define	PAD_R1			0x00000800
#define	PAD_TRIANGLE	0x00001000
#define	PAD_CIRCLE		0x00002000
#define	PAD_CROSS		0x00004000
#define	PAD_SQUARE		0x00008000
#define	PAD_TOUCH_PAD	0x00100000
#define	PAD_INTERCEPTED	0x80000000

typedef struct vec_float3
{
	float x;
	float y;
	float z;
}vec_float3;

typedef struct vec_float4
{
	float x;
	float y;
	float z;
	float w;
}vec_float4;

typedef struct ScePadData {
	unsigned int buttons;	
	uint8_t lx;
	uint8_t ly;
	uint8_t rx;
	uint8_t ry;
	uint8_t l2;
	uint8_t r2;
	uint16_t padding;
	vec_float4 quat;
	vec_float3 vel;
	vec_float3 acell;
	uint8_t touch[24];
	uint8_t connected;
	uint64_t timestamp;
	uint8_t ext[16];
	uint8_t count;
	uint8_t unknown[15];
}ScePadData;

void padInit();
void padFinish();
unsigned int padGetCurrentButtonsPressed();
void padSetCurrentButtonsPressed(unsigned int buttons);
unsigned int padGetCurrentButtonsReleased();
void padSetCurrentButtonsReleased(unsigned int buttons);
int padGetButtonHold(unsigned int filter);
int padGetButtonPressed(unsigned int filter);
int padGetButtonReleased(unsigned int filter);
int padUpdate();

#endif