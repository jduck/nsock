/*
 * things needed in nsock_unix_connect.c and nsock_unix_listen.c
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#ifndef __NSOCK_UNIX_H_
#define __NSOCK_UNIX_H_

#include "nsock.h"

#include <sys/un.h>

/*
 * the type definition for the parameter to unix* functions
 */
typedef struct __nsock_unix_stru
{
   u_char *path;       	/* backwards compatible path string */
   struct sockaddr_un sun;
   			/* optional (used if non-null) path
			 * structures */
   mode_t mode; 	/* the socket permissions */
   u_long opt;		/* options! */
   u_char *ebuf;       	/* a pointer to an error buffer */
   u_short ebl; 	/* the length of the error buffer */
} nsockunix_t;


/*
 * the different bitwise options for UNIX connections..
 */
#define NSUNX_NONE 		0x00000000 	/* no flags */
#define NSUNX_USE_UMASK        	0x00000001 	/* don't set the mode */


/*
 * nsock API routine prototypes.. 
 */
extern 	void 	nsock_unix_free(nsockunix_t **);

extern 	int    	nsock_unix_connect(nsockunix_t *);
extern 	int    	nsock_unix_listen(nsockunix_t *, int);

extern 	nsockunix_t *nsock_unix_init_listen(u_char *, u_long, u_char *, int);
extern 	nsockunix_t *nsock_unix_init_connect(u_char *, u_long, u_char *, int);

#endif
