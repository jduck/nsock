/*
 * small thing to free anything that is allocated (not to be used with
 * non-allocated structs)
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"

#include <stdlib.h>

void
nsock_free(nsp)
   nsock_t **nsp;
{
   nsock_t *ns;
   
   if (!nsp)
     return;
   ns = *nsp;
   if (ns->domain == PF_UNIX)
     {
	free(ns->unix_path);
	ns->unix_path = NULL;
     }
   else if (ns->domain == PF_INET
#ifdef INET6
	    || ns->domain == PF_INET6
#endif
	    )
     {
	free(ns->inet_to);
	ns->inet_to = NULL;
	free(ns->inet_from);
	ns->inet_from = NULL;
     }
   if (ns->state >= NSS_SOCKET_OBTAINED)
     nsock_close(ns);
   free(ns);
   *nsp = NULL;
}
