/*
 * a nice function for connecting via stream socket in inet domain
 *
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <string.h>


/*
 * connect an inialized socket
 * 
 * this version will obtain the socket, bind it to the
 * source host, and connect the socket
 */
int
nsock_connect_out(ns)
   nsock_t *ns;
{
   /* try to setup the socket */
   if (nsock_socket(ns) != NSERR_SUCCESS)
     return -1;
   
   /* bind if necessary */
   if ((ns->inet_fin.ss_family == PF_INET
#ifdef INET6
       || ns->inet_fin.ss_family == PF_INET6
#endif
	) && nsock_bind(ns) != NSERR_SUCCESS)
     return -1;

   /* connect now... */
   if (nsock_connect(ns) != NSERR_SUCCESS)
     return -1;
   return NSERR_SUCCESS;
}
