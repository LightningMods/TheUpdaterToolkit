#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kernel.h>
#include <kernel_ex.h>
#include <sysmodule_ex.h>
#include <system_service_ex.h>

#include "md5.h"
#include "util.h"

extern uint8_t orbisswu_patch[];
extern int32_t orbisswu_patch_size;

int (*sceKernelGetSystemSwVersion)(SceFwInfo* fw_info) = NULL;
int (*sceSysUtilSendSystemNotificationWithText)(int messageType, const char* message) = NULL;

struct hash_list hash_lists[] = {
	{"01.75", "a5234c6e8d37a57b374e24171173fbdd"},
	{"05.05", "f86d4f9d2c049547bd61f942151ffb55"},
	{"05.05", "f86d4f9d2c049547bd61f942151ffb55"},
	{"05.05REC", "c2a602174f6b1d8ef599640cd276924a"},
};

// Initialize the util system
void utilInit() {
	int kernel_id = sceKernelLoadStartModule("libkernel.sprx", 0, NULL, 0, NULL, NULL);

	sceKernelDlsym(kernel_id, "sceKernelGetSystemSwVersion", &sceKernelGetSystemSwVersion);
}

// Get the firmware version on the kernel (but can be spoofed !)
void firmware_version_kernel(char* version) {
	SceFwInfo fw_info;
	sceKernelGetSystemSwVersion(&fw_info);
	snprintf(version, 0x1C, "%s", fw_info.version_string);
}

// Get the firmware version by libc (for prevent from kernel spoof)
int firmware_version_libc(char* version)
{
	char fw[2] = {0};

	char* sandbox_path[50];
	char* sandbox_word = sceKernelGetFsSandboxRandomWord();
	snprintf(sandbox_path, 50, "/%s/system/common/lib/libc.sprx", sandbox_word);
    int fd = sceKernelOpen(sandbox_path, 0, 0);
    if (!fd)
    	fd = sceKernelOpen("/system/common/lib/libc.sprx", 0, 0);

    if (fd) {
		sceKernelLseek(fd, 0x374, SEEK_CUR);
		sceKernelRead(fd, &fw, 2);
	    sceKernelClose(fd);

		sprintf(version, "%02x.%02x", fw[1], fw[0]);

		return 0;
	} else {
		return 1;
	}
}

// Get the PUP Information by this hash
int pup_version_by_hash(char* path, char* version)
{
    int fd = sceKernelOpen(path, 0, 0);
    if (fd <= 0) {
    	snprintf(version, 50, "PUP Not found");
    	return 2;
	}

	// Get the PUP on Memory
	int size = sceKernelLseek(fd, 0, SEEK_END);
	sceKernelLseek(fd, 0, SEEK_SET);

	void* buffer = NULL;
	int ret = sceKernelMmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0, &buffer);
	sceKernelClose(fd);

	if (buffer == NULL) {
		snprintf(version, 50, "PUP can't be mapped. (fd: %i - mmap: %08x - size: %i)", fd, ret, size);
		return 3;
	}

	// Create MD5 hash
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, buffer, size);

	unsigned char digest[16] = {0};
	MD5_Final(digest, &ctx);

	char md5_string[33] = {0};

	for(int i = 0; i < 16; ++i) {
	    sprintf(&md5_string[i*2], "%02x", (unsigned int)digest[i]);
	}
	md5_string[32] = 0;

	for (int y = 0; y < (sizeof(hash_lists) / sizeof(hash_lists[0])); y++) {
		if (strcmp(md5_string, hash_lists[y].hash) == 0) {
			snprintf(version, 50, "%s (%s)", hash_lists[y].version, hash_lists[y].hash);
			return 0;
		}
	}

	sceKernelMunmap(buffer, size);

    snprintf(version, 50, "PUP is not found on the database.");
	return 1;
}

// Copy file
int copy_file(char* source, char* destination) {
	printf("Copying %s to %s ...\n", source, destination);

	int fd_src = sceKernelOpen(source, 0x0000, 0777);
	int fd_dst = sceKernelOpen(destination, 0x0001 | 0x0200 | 0x0400, 0777);
	if (fd_src < 0 || fd_dst < 0)
		return 1;

	int data_size = sceKernelLseek(fd_src, 0, SEEK_END);
	sceKernelLseek(fd_src, 0, SEEK_SET);

	void* data = NULL;

	sceKernelMmap(0, data_size, PROT_READ, MAP_PRIVATE, fd_src, 0, &data);
	sceKernelWrite(fd_dst, data, data_size);
	sceKernelMunmap(data, data_size);

	sceKernelClose(fd_dst);
	sceKernelClose(fd_src);

	return 0;
}

// Clean the update folder
void clean_update_folder(void) {
	printf("Cleanup /update folder ...\n");

	// Remove old update and bypass
	sceKernelRmdir("/update/PS4UPDATE.PUP");
	sceKernelUnlink("/update/PS4UPDATE.PUP");
}

// Launch the OrbisSWU Updater
void launch_update(void) {
	printf("Launching update ....\n");

	// Write OrbisSWU patch to /data !
	int fd = sceKernelOpen("/data/orbis_swu_patch.self", 0x0001 | 0x0200 | 0x0400, 0777);

	if (fd > 0) {
		sceKernelWrite(fd, orbisswu_patch, orbisswu_patch_size);
		sceKernelClose(fd);

		// Launch OrbisSWU (Patched version)
		sceSystemServiceLoadExec("/data/orbis_swu_patch.self", NULL);
	}
}
