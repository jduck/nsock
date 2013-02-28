/*
 * low-level code for connecting to resolving a unix socket
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>


int
nsock_unix_resolve(path, ns)
   u_char *path;
   nsock_t *ns;
{
   u_char *p;
   u_char full_path[PATH_MAX + 1];
   u_int max_len;
   
   /* check out the structure passed */
   if (!ns)
     return nsock_error(ns, NSERR_NO_STORAGE);
   
   /* make sure a path was supplied */
   if (!path)
     return nsock_error(ns, NSERR_NO_DATA);
   
   /* free the path in the structure if it is set */
   if (ns->unix_path)
     {
	free(ns->unix_path);
	ns->unix_path = NULL;
     }
   
   /* see if it will fit */
   max_len = sizeof(ns->unix_sun.sun_path);
   if (strlen((char *)path) < max_len - 1)
     {
	ns->unix_sun.sun_family = PF_UNIX;
	strcpy(ns->unix_sun.sun_path, (char *)path);
#ifdef __44BSD__                                                                                                        
	ns->unix_sun.sun_len = strlen((char *)path);
#endif
	return NSERR_SUCCESS;
     }
   
   /* ok, we need to see if we can split it */
   if (!(p = (u_char *)strrchr((char *)path, '/')))
     return nsock_error(ns, NSERR_UNIX_SOCK_TOO_LONG);
   
   /* make sure the remainder will fit.. */
   if (strlen((char *)p + 1) >= max_len - 1)
     return nsock_error(ns, NSERR_UNIX_SOCK_TOO_LONG);
   
   /* make sure the path will fit */
   if ((p - path) >= PATH_MAX - 1)
     return nsock_error(ns, NSERR_UNIX_PATH_TOO_LONG);

   /* copy the part before the / to our stack buffer */
   strncpy((char *)full_path, (char *)path, (p - path));
   full_path[(p - path)] = '\0';
   
   /* copy the path and socket name to the struct */
   ns->unix_path = (u_char *)strdup((char *)full_path);
   if (!ns->unix_path)
     return nsock_error(ns, NSERR_OUT_OF_MEMORY);
   ns->unix_sun.sun_family = PF_UNIX;
   strncpy(ns->unix_sun.sun_path, (char *)p + 1, max_len - 1);
#ifdef __44BSD__
   ns->unix_sun.sun_len = strlen(path);
#endif
   return NSERR_SUCCESS;
}
