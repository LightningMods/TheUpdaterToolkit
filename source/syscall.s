.intel_syntax noprefix

.extern __error

.text

.globl custom_syscall
custom_syscall:
	xor rax, rax

.globl custom_syscall_macro
custom_syscall_macro:
	mov r10, rcx
	syscall
	ret

.globl kernel_rdmsr
kernel_rdmsr:
    mov    ecx, edi
    rdmsr
    shl    rdx, 32
    or    rax, rdx
    ret

.globl    cpu_enable_wp
cpu_enable_wp:
  mov rax, cr0
  or rax, 0x10000
  mov cr0, rax
  ret

.globl    cpu_disable_wp
cpu_disable_wp:
  mov rax, cr0
  and rax, ~0x10000
  mov cr0, rax
  ret