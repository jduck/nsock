/*
 * nsock_socket()
 * 
 * get a socket!
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"


int
nsock_socket(ns)
   nsock_t *ns;
{
   /* check our settings in the structure */
   if (!ns)
     return nsock_error(ns, NSERR_NO_STORAGE);
   
   if (ns->state >= NSS_SOCKET_OBTAINED)
     return nsock_error(ns, NSERR_SOCKET_ALREADY);
   
   if (ns->domain == PF_UNSPEC)
     return nsock_error(ns, NSERR_INVALID_DOMAIN);
   
   if (ns->type == 0)
     return nsock_error(ns, NSERR_INVALID_TYPE);

   
   /* get a socket */
   ns->sd = socket(ns->domain, ns->type, ns->protocol);
   if (ns->sd == -1)
     return nsock_error(ns, NSERR_SOCKET_FAILED);
   ns->state = NSS_SOCKET_OBTAINED;
   
   /* set socket options if specified */
   if (ns->opt & NSF_REUSE_ADDR)
     {
	u_int optval = 1;
	
	if (setsockopt(ns->sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	  {
	     nsock_close(ns);
	     return nsock_error(ns, NSERR_SOCKET_SOCKOPT_FAILED);
	  }
     }
   if (ns->opt & NSF_OOB_INLINE)
     {
	u_int optval = 1;
	
	if (setsockopt(ns->sd, SOL_SOCKET, SO_OOBINLINE, &optval, sizeof(optval)) == -1)
	  {
	     nsock_close(ns);
	     return nsock_error(ns, NSERR_SOCKET_SOCKOPT_FAILED);
	  }
     }
   return NSERR_SUCCESS;
}
