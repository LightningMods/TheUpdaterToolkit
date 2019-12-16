#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel_ex.h>

#include "freebsd.h"
#include "syscall.h"
#include "elf.h"
#include "kernel_installer.h"

extern uint8_t kernel_payload_505[];
extern int32_t kernel_payload_505_size;


extern uint8_t kernel_payload_474[];
extern int32_t kernel_payload_474_size;

// Usefull function for prevent from searching function in kernel
int kern_memcmp(const void* str1, const void* str2, size_t count)
{
    register const unsigned char *s1 = (const unsigned char*)str1;
    register const unsigned char *s2 = (const unsigned char*)str2;

    while (count-- > 0)
    {
        if (*s1++ != *s2++)
            return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}

void kern_memcpy(void *dest, void *src, size_t n) 
{ 
    char *csrc = (char *)src;
    char *cdest = (char *)dest;

    for (int i=0; i<n; i++)
        cdest[i] = csrc[i]; 
}

void* kern_memset(void *dest, int val, size_t len)
{
    unsigned char *ptr = dest;
    while (len-- > 0)
        *ptr++ = val;
    return dest;
}

// Initialize kernel payload on 5.05
int init_kernel_505(struct thread* td) {
    uint8_t* kernbase = &((uint8_t*)kernel_rdmsr(0xC0000082))[KERN_505_X_FAST_SYSCALL];
    void* kernel_map = *(void**)&kernbase[KERN_505_KERNEL_MAP];

    void* (*kmem_alloc)(struct vm_map *map, uint64_t size) = (void*)(kernbase + KERN_505_KMEM_ALLOC);

    cpu_disable_wp();

    // patch memcpy first
    *(uint8_t *)(kernbase + 0x1EA53D) = 0xEB;

    // Allow sys_dynlib_dlsym in all processes.
    *(uint16_t *)(kernbase + 0x237F3B) = 0x01C1;

    // Don't restrict dynlib information.
    *(uint64_t *)(kernbase + 0x2B2620) = 0x9090909090C3C031;

    // enable mmap of all SELF 5.05
    *(uint8_t*)(kernbase + 0x117B0) = 0xB0;
    *(uint8_t*)(kernbase + 0x117B1) = 0x01;
    *(uint8_t*)(kernbase + 0x117B2) = 0xC3;

    *(uint8_t*)(kernbase + 0x117C0) = 0xB0;
    *(uint8_t*)(kernbase + 0x117C1) = 0x01;
    *(uint8_t*)(kernbase + 0x117C2) = 0xC3;

    *(uint8_t*)(kernbase + 0x13F03F) = 0x31;
    *(uint8_t*)(kernbase + 0x13F040) = 0xC0;
    *(uint8_t*)(kernbase + 0x13F041) = 0x90;
    *(uint8_t*)(kernbase + 0x13F042) = 0x90;
    *(uint8_t*)(kernbase + 0x13F043) = 0x90;

    // Allow usage of mangled symbols in dynlib_do_dlsym().
    *(uint16_t *)(kernbase + 0x2AFB47) = 0x9090;
    *(uint16_t *)(kernbase + 0x2AFB47 + 2) = 0x9090;
    *(uint16_t *)(kernbase + 0x2AFB47 + 4) = 0x9090;

    *(uint32_t *)(kernbase + 0x19ECEB0) = 0;

    *(uint8_t*)(kernbase + 0x117B0) = 0xB0;
    *(uint8_t*)(kernbase + 0x117B1) = 0x01;
    *(uint8_t*)(kernbase + 0x117B2) = 0xC3;

    *(uint8_t*)(kernbase + 0x117C0) = 0xB0;
    *(uint8_t*)(kernbase + 0x117C1) = 0x01;
    *(uint8_t*)(kernbase + 0x117C2) = 0xC3;

    *(uint8_t*)(kernbase + 0x13F03F) = 0x31;
    *(uint8_t*)(kernbase + 0x13F040) = 0xC0;
    *(uint8_t*)(kernbase + 0x13F041) = 0x90;
    *(uint8_t*)(kernbase + 0x13F042) = 0x90;
    *(uint8_t*)(kernbase + 0x13F043) = 0x90;

    // Copyin / Copyout check bypass
    *(uint16_t *)(kernbase + 0x1EA758) = 0x9090;
    *(uint16_t *)(kernbase + 0x1EA767) = 0x9090;

    *(uint16_t *)(kernbase + 0x1EA66B) = 0x9090;
    *(uint16_t *)(kernbase + 0x1EA673) = 0x9090;
    *(uint16_t *)(kernbase + 0x1EA682) = 0x9090;

    // patch sceSblACMgrIsAllowedSystemLevelDebugging
    kern_memcpy((void *)(kernbase + 0x11730), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8);

    // patch sceSblACMgrHasMmapSelfCapability
    kern_memcpy((void *)(kernbase + 0x117B0), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8);

    // patch sceSblACMgrIsAllowedToMmapSelf
    kern_memcpy((void *)(kernbase + 0x117C0), "\x48\xC7\xC0\x01\x00\x00\x00\xC3", 8);

    // self patches
    kern_memcpy((void *)(kernbase + 0x13F03F), "\x31\xC0\x90\x90\x90", 5);

    // patch vm_map_protect check
    kern_memcpy((void *)(kernbase + 0x1A3C08), "\x90\x90\x90\x90\x90\x90", 6);

    // patch kmem_alloc
    *(uint8_t *)(kernbase + 0xFCD48) = VM_PROT_ALL;
    *(uint8_t *)(kernbase + 0xFCD56) = VM_PROT_ALL;

    cpu_enable_wp();

    uint64_t mapsize = 0;
    int (*payload_entry)(void *p);

    // calculate mapped size
    if (elf_mapped_size(kernel_payload_505, &mapsize)) {
        return 1;
    }

    // allocate memory
    uint64_t alignedSize = (mapsize + 0x3FFFull) & ~0x3FFFull;
    void* kmemory = kmem_alloc(kernel_map, alignedSize);
    if(!kmemory) {
        return 2;
    }

    // load the elf
    if (load_elf(kernel_payload_505, kernel_payload_505_size, kmemory, mapsize, (void **)&payload_entry)) {
        return 3;
    }

    // call entry
    if (payload_entry(NULL)) {
        return 4;
    }

    return 0;
}

// Initialize kernel payload on 4.74
int init_kernel_474(struct thread* td) {
    uint8_t* kernbase = &((uint8_t*)kernel_rdmsr(0xC0000082))[KERN_474_X_FAST_SYSCALL];
    void* kernel_map = *(void**)&kernbase[KERN_474_KERNEL_MAP];
    void* (*kmem_alloc)(struct vm_map *map, uint64_t size) = (void*)(kernbase + KERN_474_KMEM_ALLOC);

    cpu_disable_wp();
    // WIP
    cpu_enable_wp();

    uint64_t mapsize = 0;
    int (*payload_entry)(void *p);

    // calculate mapped size
    if (elf_mapped_size(kernel_payload_474, &mapsize)) {
        return 1;
    }

    // allocate memory
    uint64_t alignedSize = (mapsize + 0x3FFFull) & ~0x3FFFull;
    void* kmemory = kmem_alloc(kernel_map, alignedSize);
    if(!kmemory) {
        return 2;
    }

    // load the elf
    if (load_elf(kernel_payload_474, kernel_payload_474_size, kmemory, mapsize, (void **)&payload_entry)) {
        return 3;
    }

    // call entry
    if (payload_entry(NULL)) {
        return 4;
    }

    return 0;
}

// Try to jailbreak the process
int init_kernel(char* version) {
    // WIP: Version to kernel
    custom_syscall(11, init_kernel_505);

    // Test to Jailbreak with new syscall
    custom_syscall(68);

    // Check for current uid (24 => int getuid(void))
    if (custom_syscall(24) == 0) {
        return 0; // Fully Jailbreaked
    } else {
        return 1; // Jailbreaked but not root ?
    }
}
