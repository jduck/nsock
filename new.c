/*
 * small thing to return an allocated struct
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <stdlib.h>

nsock_t *
nsock_new(domain, type, protocol, err)
   int domain, type, protocol;
   u_int *err;
{
   nsock_t *ret;
   
   if (!(ret = (nsock_t *)calloc(1, sizeof(nsock_t))))
     {
	if (err)
	  *err = NSERR_OUT_OF_MEMORY;
	return NULL;
     }
   
   /* check domain and type */
   if (domain != PF_INET
#ifdef INET6
       && domain != PF_INET6
#endif
       && domain != PF_UNIX)
     {
	if (err)
	  *err = NSERR_INVALID_DOMAIN;
	free(ret);
	return NULL;
     }
   if (type != SOCK_STREAM && type != SOCK_DGRAM)
     {
	if (err)
	  *err = NSERR_INVALID_TYPE;
	free(ret);
	return NULL;
     }
   
   /* fill it out and return it */
   ret->domain = domain;
   ret->type = type;
   ret->protocol = protocol;
   return ret;
}
