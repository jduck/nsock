/*
 * close a socket, and update the struct to reflect it
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <unistd.h>

int
nsock_close(ns)
   nsock_t *ns;
{
   if (!ns)
     return nsock_error(ns, NSERR_NO_STORAGE);
   
   /* save a copy */
   if (close(ns->sd) != -1)
     {
	/* revert state information */
	ns->state = NSS_SOCKET_UNALLOC;
	ns->sd = -1;
	ns->domain = PF_UNSPEC;
	ns->type = 0;
	ns->protocol = 0;
	return NSERR_SUCCESS;
     }
#ifdef HAVE_SSL
   if (ns->opt & NSF_USE_SSL)
     {
	if (ns->ns_ssl.ssl)
	  SSL_shutdown(ns->ns_ssl.ssl);
	if (ns->ns_ssl.context)
	  {
	     SSL_CTX_free(ns->ns_ssl.context);
	     ns->ns_ssl.context = NULL;
	  }
     }
#endif
   return nsock_error(ns, NSERR_CLOSE_FAILED);
}
