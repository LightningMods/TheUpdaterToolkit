#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel_ex.h>

#include "freebsd.h"

#ifndef _KERNEL_H
#define _KERNEL_H

// 505 Kernel offset for the elf loader
#define KERN_505_X_FAST_SYSCALL	-0x1C0
#define KERN_505_KERNEL_MAP		0x1AC60E0
#define KERN_505_KMEM_ALLOC		0xFCC80

// 474 Kernel offset for the elf loader
#define KERN_474_X_FAST_SYSCALL	-0x30B7D0
#define KERN_474_KERNEL_MAP		0x1B39218
#define KERN_474_KMEM_ALLOC		0x16DF30

void kern_memcpy(void *dest, void *src, size_t n);
void* kern_memset(void *dest, int val, size_t len);
int kern_memcmp(const void* str1, const void* str2, size_t count);

int init_kernel(char* version);

#endif