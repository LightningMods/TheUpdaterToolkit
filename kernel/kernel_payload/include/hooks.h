// golden
// 6/12/2018
//

#ifndef _HOOKS_H
#define _HOOKS_H

#include <ksdk.h>
#include "proc.h"

#define SYS_PROC_ALLOC      1
#define SYS_PROC_FREE       2
#define SYS_PROC_PROTECT    3
#define SYS_PROC_VM_MAP     4
#define SYS_PROC_INSTALL    5
#define SYS_PROC_CALL       6
#define SYS_PROC_ELF        7
#define SYS_PROC_INFO       8
#define SYS_PROC_THRINFO    9

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// Update IOCTL Hook

struct ioctl_args {
     int fd;
     unsigned long com;
     void* data;
};

struct stat_args {
  char* path;
  void* sb;
};

typedef struct _decrypt_header_args
{
  void* buffer;
  uint64_t length;
  int type;
}
decrypt_header_args;

typedef struct _verify_segment_args
{
  uint16_t index;
  void* buffer;
  uint64_t length;
}
verify_segment_args;

typedef struct _decrypt_segment_args
{
  uint16_t index;
  void* buffer;
  uint64_t length;
}
decrypt_segment_args;

typedef struct _decrypt_segment_block_args
{
  uint16_t entry_index;
  uint16_t block_index;
  void* block_buffer;
  uint64_t block_length;
  void* table_buffer;
  uint64_t table_length;
}
decrypt_segment_block_args;

void* get_syscall_function(uint32_t n);
void install_syscall(uint32_t n, void *func);
int install_hooks();
void klog_thread(void* args);

#endif
