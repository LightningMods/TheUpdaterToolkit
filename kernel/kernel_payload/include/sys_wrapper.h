#ifndef _SYS_WRAP_H
#define _SYS_WRAP_H

#include <ksdk.h>

#define O_RDONLY  0x0000    /* open for reading only */
#define O_WRONLY  0x0001    /* open for writing only */
#define O_RDWR    0x0002    /* open for reading and writing */
#define O_ACCMODE 0x0003    /* mask for above modes */
#define O_NONBLOCK 0x0004    /* no delay */
#define O_APPEND  0x0008    /* set append mode */
#define O_SHLOCK  0x0010    /* open with shared file lock */
#define O_EXLOCK  0x0020    /* open with exclusive file lock */
#define O_ASYNC   0x0040    /* signal pgrp when data ready */
#define O_FSYNC   0x0080    /* synchronous writes */
#define O_CREAT   0x0200    /* create if nonexistant */
#define O_TRUNC   0x0400    /* truncate to zero length */
#define O_EXCL    0x0800    /* error if already exists */

#define AF_INET		2	/* Internet IP Protocol 	*/
#define SOCK_STREAM	1		/* stream (connection) socket	*/
#define INADDR_ANY ((unsigned long int) 0x00000000)

#define SO_SNDBUF	0x1001		/* send buffer size */
#define SO_RCVBUF	0x1002		/* receive buffer size */
#define SO_SNDLOWAT	0x1003		/* send low-water mark */
#define SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	0x1005		/* send timeout */
#define SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR	0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */

#define	SOL_SOCKET	0xffff		/* options for socket level */

#define bswap16(x) ((uint16_t)((((uint16_t) (x) & 0xff00) >> 8) | \
                               (((uint16_t) (x) & 0x00ff) << 8)))

// sockaddr
struct sockaddr {
  unsigned char sa_len;   /* total length */
  unsigned char sa_family;  /* address family */
  char sa_data[14];  /* actually longer; address value */
};

/* Internet address. */
struct in_addr {
  uint32_t s_addr;     /* address in network byte order */
};

// sockaddr_in
struct sockaddr_in {
  uint8_t   sin_len;
  uint8_t   sin_family;
  uint16_t  sin_port;
  struct  in_addr sin_addr;
  char  sin_zero[8];
};


// Open/Read/Write/Close and socket args
struct open_args {
  char* path;
  int flags;
  int mode;
};

struct read_args {
  int fd;
  void* buf;
  uint64_t nbyte;
};

struct write_args {
  int fd;
  void* buf;
  uint64_t nbyte;
};

struct close_args {
  uint64_t fd;
};

struct lseek_args {
  int fd;
  long offset;
  int whence;
};

struct socket_args {
  uint64_t domain;
  uint64_t type;
  uint64_t protocol;
};

struct bind_args {
  uint64_t s;
  uint64_t name;
  uint64_t namelen;
};

struct listen_args {
  uint64_t s;
  uint64_t backlog;
};

struct accept_args {
  uint64_t s;
  uint64_t name;
  uint64_t anamelen;
};

struct shutdown_args {
  uint64_t s;
  uint64_t how;
};

struct setsockopt_args {
	int s;
	int level;
	int name;
	void *val;
	int valsize;
};

void initSysWrapper();
int open(char* path, int flags, int mode);
int close(int fd);
long lseek(int fd, long offset, int whence);
int read(int fd, void* buf, uint64_t nbyte);
int write(int fd, void* buf, uint64_t nbyte);
int socket(int domain, int type, int protocol);
int bind(int s, struct sockaddr *name, int namelen);
int listen(int s, int backlog);
int accept(int s, struct sockaddr *name, uint32_t *anamelen);
int shutdown(int s, int how);
int setsockopt(int s, int level, int name, void* val, int valsize);

#endif
