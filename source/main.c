#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#include "freebsd.h"
#include "kernel_installer.h"
#include "syscall.h"
#include "pad.h"
#include "video.h"
#include "util.h"

#include "view.h"
#include "view/credit.h"
#include "view/main.h"

int homebrewRun = 1;
int64_t flipArg=0;

View mainView = {.init = main_init, .update = main_update, .render = main_render };
View creditView = {.init = credit_init, .update = credit_update, .render = credit_render };

View* currentView = &mainView;

void init() {
	// Resolve all function
	videoInit();
	padInit();
	utilInit();

	// Jailbreak after all resolved function
	/*
	char* firmware_version[5] = {0};
	firmware_version_libc(firmware_version);
	init_kernel(firmware_version);
	*/

	currentView->init();
}

void finish() {
	padFinish();
	videoFinish();
}

int update() {
	padUpdate();

	if (currentView) {
		int status = currentView->update();
		switch (status) {
			case 1: // Stop the homebrew
				homebrewRun = 0;
				return 0;
			case 13371: // Lanching OrbisSWU
				homebrewRun = -1;
				return 0;
		}
	}

	return 1;
}

int render() {
	videoStartDrawing();
	videoClearBuffer(0);

	if (currentView) {
		currentView->render();
	}

	videoFinishDrawing(flipArg);
	videoSwapBuffers();
	flipArg++;

	return 0;
}

static void cleanup(void) {
	int ret;

	ret = sceSysmoduleUnloadModuleInternal(SCE_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
	if (ret) {
		printf("sceSysmoduleUnloadModuleInternal(%s) failed: 0x%08X\n", "SCE_SYSMODULE_INTERNAL_SYSTEM_SERVICE", ret);
	}
}

int main(int argc, const char* const argv[]) {

	atexit(&cleanup);
	int ret = sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
	if (ret) {
		printf("sceSysmoduleLoadModuleInternal(%s) failed: 0x%08X\n", "SCE_SYSMODULE_INTERNAL_SYSTEM_SERVICE", ret);
		goto err;
	}

	sceSystemServiceEnableSuspendConfirmationDialog();
	sceSystemServiceHideSplashScreen();

	init();
	while (homebrewRun) {
		if (update())
			render();
	}
	finish();

	if (homebrewRun < 0) {
		// Init the util system
		utilInit();
		init_kernel("5.05");

		// Cleanup the Update folder
		clean_update_folder();

		// Copy the update to the /update folder
		if (copy_file("/mnt/usb0/PS4UPDATE.PUP", "/update/PS4UPDATE.PUP"))
			copy_file("/mnt/usb1/PS4UPDATE.PUP", "/update/PS4UPDATE.PUP");

		// Launch the update !
		launch_update();
	}

err:
	return 0;
}


