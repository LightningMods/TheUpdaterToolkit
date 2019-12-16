#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#ifndef _UTIL_H
#define _UTIL_H

typedef struct {
	uint64_t unk1;
	char version_string[0x1C];
	uint32_t version;
} SceFwInfo;

struct hash_list {
	char version[50];
	char hash[50];
};

void utilInit();
void firmware_version_kernel(char* version);
int firmware_version_libc(char* version);
int pup_version_by_hash(char* path, char* version);
void clean_update_folder(void);
int copy_file(char* source, char* destination);
void launch_update(void);

#endif