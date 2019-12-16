#pragma once

#ifndef VIEWH
#define VIEWH

typedef struct {
	int (*init)(void);
	int (*update)(void);
	int (*render)(void);
	void* data;
} View;

extern View* currentView;

/////////////////
// Define View //
/////////////////

extern View mainView;
extern View creditView;

#endif
