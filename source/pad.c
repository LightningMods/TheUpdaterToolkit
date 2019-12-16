// Based on pad from liborbis
// https://github.com/psxdev/liborbis/blob/master/libpad/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#include "pad.h"

int pad_id;
int userservice_id;

SceUserServiceUserId userId;
ScePadData *padDataCurrent;
ScePadData *padDataLast;
unsigned int buttonsPressed;
unsigned int buttonsReleased;
unsigned int buttonsHold;
int padHandle;

int (*sceUserServiceInitialize)(int *params) = NULL;
int (*sceUserServiceTerminate)() = NULL;
int (*sceUserServiceGetInitialUser)(SceUserServiceUserId *userId) = NULL;
int (*scePadInit)(void) = NULL;
int (*scePadOpen)(SceUserServiceUserId userId,int type,int index,uint8_t *param) = NULL;
int (*scePadClose)(int handle) = NULL;
int (*scePadRead)(int handle, ScePadData *data, int count) = NULL;
int (*scePadReadState)(int handle, ScePadData *data) = NULL;


void padInit() {
	int ret;
    int param=700;

	userservice_id = sceKernelLoadStartModule("libSceUserService.sprx", 0, NULL, 0, NULL, NULL);
	pad_id = sceKernelLoadStartModule("libScePad.sprx", 0, NULL, 0, NULL, NULL);

	sceKernelDlsym(userservice_id, "sceUserServiceInitialize", &sceUserServiceInitialize);
	sceKernelDlsym(userservice_id, "sceUserServiceTerminate", &sceUserServiceTerminate);
	sceKernelDlsym(userservice_id, "sceUserServiceGetInitialUser", &sceUserServiceGetInitialUser);
    sceKernelDlsym(pad_id, "scePadInit", &scePadInit);
    sceKernelDlsym(pad_id, "scePadOpen", &scePadOpen);
	sceKernelDlsym(pad_id, "scePadReadState", &scePadReadState);
	sceKernelDlsym(pad_id, "scePadRead", &scePadRead);
	sceKernelDlsym(pad_id, "scePadClose", &scePadClose);

	padDataCurrent = (ScePadData*)malloc(sizeof(ScePadData));
	padDataLast = (ScePadData*)malloc(sizeof(ScePadData));

	ret = sceUserServiceInitialize(&param);

	scePadInit();
	ret = sceUserServiceGetInitialUser(&userId);

	padHandle = scePadOpen(userId, 0, 0, NULL);
}

void padFinish() {
	scePadClose(padHandle);

	sceKernelStopUnloadModule(pad_id, 0, NULL, 0, NULL, NULL);
	sceKernelStopUnloadModule(userservice_id, 0, NULL, 0, NULL, NULL);
}

unsigned int padGetCurrentButtonsPressed()
{
	return buttonsPressed;
}

void padSetCurrentButtonsPressed(unsigned int buttons)
{
	buttonsPressed=buttons;
}

unsigned int padGetCurrentButtonsReleased()
{
	return buttonsReleased;
}

void padSetCurrentButtonsReleased(unsigned int buttons)
{
	buttonsReleased=buttons;
}


int padGetButtonHold(unsigned int filter)
{
	if((buttonsHold&filter)==filter)
	{
		return 1;
	}
	return 0;
}

int padGetButtonPressed(unsigned int filter)
{
	if((buttonsPressed&filter)==filter)
	{
		return 1;
	}
	return 0;
}

int padGetButtonReleased(unsigned int filter)
{
 	if((buttonsReleased&filter)==filter)
	{
		if(~(padDataLast->buttons)&filter)
		{
			return 0;
		}
		return 1;
	}
	return 0;
}

int padUpdate()
{
	int ret;
	unsigned int actualButtons=0;
	unsigned int lastButtons=0;
	memcpy(padDataLast, padDataCurrent, sizeof(ScePadData));
	
	ret = scePadReadState(padHandle, padDataCurrent);
	
	if(ret==0 && padDataCurrent->connected==1)
	{
		actualButtons = padDataCurrent->buttons;
		lastButtons= padDataLast->buttons;
		buttonsPressed = (actualButtons)&(~lastButtons);
		if(actualButtons != lastButtons)
		{
			buttonsReleased = (~actualButtons)&(lastButtons);
		}
		else
		{
			buttonsReleased = 0;
			
		}
		buttonsHold = actualButtons&lastButtons;
		return 0;
		
	}
	else
	{
		return -1;
	}
}