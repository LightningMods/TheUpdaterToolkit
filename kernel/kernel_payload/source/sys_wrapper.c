#include "hooks.h"
#include "sys_wrapper.h"

int (*sys_open)(struct thread *td, struct open_args *uap) = NULL;
int (*sys_close)(struct thread *td, struct close_args *uap) = NULL;
int (*sys_read)(struct thread *td, struct read_args *uap) = NULL;
int (*sys_write)(struct thread *td, struct write_args *uap) = NULL;
int (*sys_lseek)(struct thread *td, struct lseek_args *uap) = NULL;
int (*sys_socket)(struct thread *td, struct socket_args *uap) = NULL;
int (*sys_bind)(struct thread* td, struct bind_args* uap) = NULL;
int (*sys_listen)(struct thread* td, struct listen_args* uap) = NULL;
int (*sys_accept)(struct thread* td, struct accept_args* uap) = NULL;
int (*sys_shutdown)(struct thread* td, struct shutdown_args* uap) = NULL;
int (*sys_setsockopt)(struct thread* td, struct setsockopt_args* uap) = NULL;

void initSysWrapper() {
	sys_open = get_syscall_function(5);
	sys_close = get_syscall_function(6);
	sys_read = get_syscall_function(3);
	sys_write = get_syscall_function(4);
	sys_lseek = get_syscall_function(478);
	sys_socket = get_syscall_function(97);
	sys_bind = get_syscall_function(104);
	sys_listen = get_syscall_function(106);
	sys_accept = get_syscall_function(99);
	sys_shutdown = get_syscall_function(134);
	sys_setsockopt = get_syscall_function(105);
}

int open(char* path, int flags, int mode) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct open_args open;
    open.path = path;
    open.flags = flags;
    open.mode = mode;

    int error = sys_open(td, &open);
    if (error)
        return -error;

    return td->td_retval[0];
}

int close(int fd) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct close_args close;
    close.fd = fd;

    int error = sys_close(td, &close);
    if (error)
        return -error;

    return td->td_retval[0];
}

long lseek(int fd, long offset, int whence) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct lseek_args lseek;
    lseek.fd = fd;
    lseek.offset = offset;
    lseek.whence = whence;

    int error = sys_lseek(td, &lseek);
    if (error)
        return -error;

    return td->td_retval[0];
}

int read(int fd, void* buf, uint64_t nbyte) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct read_args read;
    read.fd = fd;
    read.buf = buf;
    read.nbyte = nbyte;

    int error = sys_read(td, &read);
    if (error)
        return -error;

    return td->td_retval[0];
}

int write(int fd, void* buf, uint64_t nbyte) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct write_args write;
    write.fd = fd;
    write.buf = buf;
    write.nbyte = nbyte;

    int error = sys_write(td, &write);
    if (error)
        return -error;

    return td->td_retval[0];
}

int socket(int domain, int type, int protocol) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct socket_args skt;
    skt.domain = domain;
    skt.type = type;
    skt.protocol = protocol;

	sys_socket(td, &skt);

	/*
    int error = 
    if (error)
        return -error;
	*/

    return td->td_retval[0];
}

int bind(int s, struct sockaddr *name, int namelen) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct bind_args bind;
    bind.s = s;
    bind.name = (uint64_t)name;
    bind.namelen = namelen;

    int error = sys_bind(td, &bind);
    if (error)
        return -error;

    return td->td_retval[0];
}

int listen(int s, int backlog) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct listen_args listen;
    listen.s = s;
    listen.backlog = backlog;

    int error = sys_listen(td, &listen);
    if (error)
        return -error;

    return td->td_retval[0];
}

int accept(int s, struct sockaddr *name, uint32_t *anamelen) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct accept_args accept;
    accept.s = s;
    accept.name = (uint64_t)name;
    accept.anamelen = (uint64_t)anamelen;

    int error = sys_accept(td, &accept);
    if (error)
        return -error;

    return td->td_retval[0];
}

int shutdown(int s, int how) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct shutdown_args shutdown;
    shutdown.s = s;
    shutdown.how = how;

    int error = sys_shutdown(td, &shutdown);
    if (error)
        return -error;

    return td->td_retval[0];
}

int setsockopt(int s, int level, int name, void* val, int valsize) {
    struct thread* td = curthread();
    td->td_retval[0] = 0;

    struct setsockopt_args setsockopt;
    setsockopt.s = s;
    setsockopt.level = level;
    setsockopt.name = name;
    setsockopt.val = val;
    setsockopt.valsize = valsize;

    int error = sys_setsockopt(td, &setsockopt);
    if (error)
        return -error;

    return td->td_retval[0];
}
