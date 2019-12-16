#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	SYSCALL(name, number)	\
	__asm__(".intel_syntax noprefix"); \
	__asm__(".globl " #name ""); \
	__asm__("" #name ":"); \
	__asm__("movq rax, " #number ""); \
	__asm__("jmp custom_syscall_macro"); \

unsigned long custom_syscall(unsigned long n, ...);
void cpu_disable_wp(void);
void cpu_enable_wp(void);
uint64_t kernel_rdmsr(uint32_t reg);