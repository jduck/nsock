/*
 * things needed in nsock_tcp_listen.c and nsock_tcp_connect.c
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */
#ifndef __NSOCK_TCP_H_
#define __NSOCK_TCP_H_

#include "nsock.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * the type definition for the parameter to tcp* functions
 */
typedef struct __nsock_tcp_stru
{
   u_char *from, *to;	/* backwards compatible strings */
   struct sockaddr_in fin, tin;
   			/* optional (used if non-null) from/to 
			 * address structures */
   u_long opt;		/* options! */
   u_char *ebuf;       	/* a pointer to an error buffer */
   u_short ebl; 	/* the length of the error buffer */
} nsocktcp_t;


/*
 * the different bitwise options for TCP connections..
 */
#define NSTCP_NONE 		0x00000000
#define NSTCP_NON_BLOCK        	0x00000001
#define NSTCP_RAND_SRC_PORT 	0x00000002
#define NSTCP_REUSE_ADDR 	0x00000004

/* unimplemented options */
#define NSTCP_SRC_ROUTE        	0x00000008


/*
 * nsock API routine prototypes.. 
 */
extern 	u_char * 	nsock_tcp_host(u_char *, u_short);
extern 	void 		nsock_tcp_free(nsocktcp_t **);

extern 	nsocktcp_t 	*nsock_tcp_init_connect(u_char *, u_char *, u_long, u_char *, int);
extern 	int    		nsock_tcp_connect(nsocktcp_t *, int);
#ifdef NON_BLOCKING_CONNECTS
extern 	int    		nsock_tcp_connected(int);
extern 	int 		nsock_tcp_set_blocking(int, int);
#endif

extern 	nsocktcp_t	*nsock_tcp_init_listen(u_char *, u_long, u_char *, int);
extern 	int    		nsock_tcp_listen(nsocktcp_t *, int);

#endif
