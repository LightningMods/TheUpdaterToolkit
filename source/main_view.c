#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#include "pad.h"
#include "video.h"
#include "view.h"
#include "view/main.h"

int main_init(void) {

	return 0;
}

int main_update(void) {
	if (padGetCurrentButtonsPressed(PAD_CROSS)) {
		return 13371;
	}

	return 13371;
}

int main_render(void) {
	videoDebugDrawString(0, 0, 1, "-- TheUpdaterLoader 1.0");
	return 0;
}
