/* golden */
/* 1/2/2018 */

// contains function prototypes, contants, and structures for freebsd (specifcally PS4)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel_ex.h>

#ifndef _FREEBSD_H
#define _FREEBSD_H

#include "sparse.h"

#define	VM_PROT_NONE		0x00
#define VM_PROT_READ		0x01
#define VM_PROT_WRITE		0x02
#define VM_PROT_EXECUTE		0x04
#define VM_PROT_DEFAULT		(VM_PROT_READ | VM_PROT_WRITE)
#define VM_PROT_ALL			(VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE)
#define VM_PROT_NO_CHANGE	0x08
#define VM_PROT_COPY		0x10
#define VM_PROT_WANTS_COPY	0x10

#define X86_CR0_WP (1 << 16)

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((uint64_t)(!(sizeof(x) % sizeof(0[x])))))

static inline __attribute__((always_inline))
void* kcurthread(void)
{
    uint64_t td;
    __asm__ ("movq %0, %%gs:0" : "=r" (td) : : "memory");
    return (void*)td;
}

typedef uint64_t vm_offset_t;

#define	TAILQ_FIRST(head) ((head)->tqh_first)
#define	TAILQ_NEXT(elm, field) ((elm)->field.tqe_next)

#define	TAILQ_HEAD(name, type)									\
struct name {													\
	struct type *tqh_first;	/* first element */					\
	struct type **tqh_last;	/* addr of last next element */		\
	TRACEBUF													\
}

#define	TAILQ_ENTRY(type)											\
struct {															\
	struct type *tqe_next;	/* next element */						\
	struct type **tqe_prev;	/* address of previous next element */	\
	TRACEBUF														\
}

#define	LIST_ENTRY(type)											\
struct {															\
	struct type *le_next;	/* next element */						\
	struct type **le_prev;	/* address of previous next element */	\
}

#define	TAILQ_FOREACH(var, head, field)				\
	for ((var) = TAILQ_FIRST((head));				\
	    (var);										\
(var) = TAILQ_NEXT((var), field))

struct qm_trace {
	char * lastfile;
	int lastline;
	char * prevfile;
	int prevline;
};

TYPE_BEGIN(struct uio, 0x30);
TYPE_FIELD(uint64_t uio_iov, 0);
TYPE_FIELD(uint32_t uio_iovcnt, 8);
TYPE_FIELD(uint64_t uio_offset, 0x10);
TYPE_FIELD(uint64_t uio_resid, 0x18);
TYPE_FIELD(uint32_t uio_segflg, 0x20);
TYPE_FIELD(uint32_t uio_rw, 0x24);
TYPE_FIELD(struct thread *uio_td, 0x28);
TYPE_END();

TYPE_BEGIN(struct sysent, 0x30);
TYPE_FIELD(uint32_t sy_narg, 0x00);
TYPE_FIELD(void *sy_call, 0x08);
TYPE_FIELD(uint16_t sy_auevent, 0x10);
TYPE_FIELD(uint64_t sy_systrace_args_func, 0x18);
TYPE_FIELD(uint32_t sy_entry, 0x20);
TYPE_FIELD(uint32_t sy_return, 0x24);
TYPE_FIELD(uint32_t sy_flags, 0x28);
TYPE_FIELD(uint32_t sy_thrcnt, 0x2C);
TYPE_END();

struct lock_object {
	const char* lo_name;
	uint32_t lo_flags;
	uint32_t lo_data;
	void* lo_witness;
};

struct mtx {
	struct lock_object lock_object;
	volatile void* mtx_lock;
};

struct sx {
	struct lock_object lock_object;
	volatile uintptr_t sx_lock;
};

TYPE_BEGIN(struct vm_map_entry, 0xC0);
TYPE_FIELD(struct vm_map_entry *prev, 0);
TYPE_FIELD(struct vm_map_entry *next, 8);
TYPE_FIELD(struct vm_map_entry *left, 0x10);
TYPE_FIELD(struct vm_map_entry *right, 0x18);
TYPE_FIELD(vm_offset_t start, 0x20);
TYPE_FIELD(vm_offset_t end, 0x28);
TYPE_FIELD(vm_offset_t offset, 0x50);
TYPE_FIELD(uint16_t prot, 0x5C);
TYPE_FIELD(char name[32], 0x8D);
TYPE_END();

TYPE_BEGIN(struct vm_map, 0x178);
TYPE_FIELD(struct vm_map_entry header, 0);
TYPE_FIELD(struct sx lock, 0xB8);
TYPE_FIELD(struct mtx system_mtx, 0xD8);
TYPE_FIELD(int nentries, 0x100);
TYPE_END();

TYPE_BEGIN(struct vmspace, 0x250);
TYPE_FIELD(struct vm_map vm_map, 0);
// maybe I will add more later just for documentation purposes
TYPE_END();

struct auditinfo_addr {
	uint8_t useless[184];
};

struct ucred {
	uint32_t cr_ref;					// reference count		0x0000
	uint32_t cr_uid;					// effective user id	0x0004
	uint32_t cr_ruid;					// real user id			0x0008
	uint32_t useless2;					// 						0x000C
	uint32_t useless3;					//
	uint32_t cr_rgid;					// real group id
	uint32_t useless4;					//
	void *useless5;						//
	void *useless6;						//
	void *cr_prison;					// jail(2)				0x0030
	void *useless7;						//
	uint32_t useless8;					//
	void *useless9[2];					//
	void *useless10;					//
	struct auditinfo_addr cr_audit;		//
	uint32_t *cr_groups;				// groups
	uint32_t useless12;					//
};

struct filedesc {
	void *useless1[3];
	void *fd_rdir;
	void *fd_jdir;
};

TYPE_BEGIN(struct proc, 0x800); // XXX: random, don't use directly without fixing it
TYPE_FIELD(struct proc *p_forw, 0);
TYPE_FIELD(TAILQ_HEAD(, thread) p_threads, 0x10);
TYPE_FIELD(struct ucred *p_ucred, 0x40);
TYPE_FIELD(struct filedesc *p_fd, 0x48);
TYPE_FIELD(int pid, 0xB0);
TYPE_FIELD(struct vmspace *p_vmspace, 0x168);
TYPE_FIELD(char titleId[10], 0x390);
TYPE_FIELD(char p_comm[32], 0x44C);
TYPE_END();


TYPE_BEGIN(struct thread, 0x3D8); // XXX: random, don't use directly without fixing it
TYPE_FIELD(struct mtx *volatile td_lock, 0);
TYPE_FIELD(struct proc *td_proc, 8);
TYPE_FIELD(TAILQ_ENTRY(thread) td_plist, 0x10);
TYPE_FIELD(int tid, 0x88);
TYPE_FIELD(int td_pinned, 0x12C);
TYPE_FIELD(struct ucred *td_ucred, 0x130);
TYPE_FIELD(char td_name[32], 0x284);
TYPE_FIELD(uint64_t td_retval[2], 0x398);
TYPE_FIELD(uint16_t td_priority, 0x380);
TYPE_END();

static inline struct thread *curthread(void) {
	struct thread* td;
	__asm__ __volatile__ (
	    "mov %0, %%gs:0"
	    : "=r"(td)
	);

	return td;
}

#endif
