/*
 * includes things needed for sockets in unix
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#ifndef __NSOCK_H_
#define __NSOCK_H_

#include "nsock_defs.h"

/* generic socket includes */
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>

/* this is the default connect timeout (seconds) */
#define CONNECT_TIMEOUT 10

/* Are we doing non blocking connects ? */
#if defined(NBLOCK_POSIX) || defined(NBLOCK_BSD) || defined(NBLOCK_SYSV)
# define NON_BLOCKING_CONNECTS
#endif

/* generic prototypes.. */
extern 	ssize_t 	nsock_read(int, void *, size_t, u_int);
extern 	ssize_t 	nsock_readln(int, void *, size_t, u_int);
extern 	ssize_t 	nsock_write(int, void *, size_t, u_int);
extern 	ssize_t        	nsock_fdprintf(int, u_int, const u_char *, ...);

#endif
