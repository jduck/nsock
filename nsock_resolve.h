/*
 * things needed to resolve hosts..
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */
#ifndef __NSOCK_RESOLVE_H_
#define __NSOCK_RESOLVE_H_

#include "nsock.h"

#include <netinet/in.h>

/* possible return values */
#define NSOCK_R_SUCCESS 	0
#define NSOCK_R_NOT_FOUND	-1
#define NSOCK_R_NO_DATA 	-2
#define NSOCK_R_NO_STORE 	-3
#define NSOCK_R_BAD_PORT 	-4
#define NSOCK_R_BAD_HOST 	-5


/*
 * nsock API routine prototypes..
 */
extern 	int    	nsock_resolve(u_char *, struct sockaddr_in *);

#endif
