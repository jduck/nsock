/*
 * nsock_listen()
 * 
 * listen on a socket!
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"


int
nsock_listen(ns)
   nsock_t *ns;
{
   int listen_ret;
   
   /* check our settings in the structure */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_BOUND) != NSERR_SUCCESS)
     return -1;
   
   /* is it right? (either UNIX or INET are allowed here) */
   if (ns->domain != PF_UNIX 
       && ns->domain != PF_INET
#ifdef INET6
       && ns->domain != PF_INET6
#endif
       )
     return nsock_error(ns, NSERR_INVALID_DOMAIN);
   
   if (ns->type != SOCK_STREAM)
     return nsock_error(ns, NSERR_INVALID_TYPE);
   
   /* dont bother checking backlog... */
   
   /* try to listen*/
   listen_ret = listen(ns->sd, ns->backlog);
   if (listen_ret == -1)
     {
	nsock_close(ns);
	return nsock_error(ns, NSERR_LISTEN_FAILED);
     }
   ns->state = NSS_SOCKET_LISTENING;
   return NSERR_SUCCESS;
}
