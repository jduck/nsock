/*
 * nice way of listening on a stream socket
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <string.h>


static int nsock_inet_listen_parse(nsock_t *, u_char *);
static int nsock_unix_listen_parse(nsock_t *, u_char *);


nsock_t *
nsock_listen_init(domain, type, addr, backlog, opts, err)
   int domain, type;
   u_char *addr;
   int backlog;
   u_long opts;
   u_int *err;
{
   nsock_t *ns;
   
   /* make sure we have an address */
   if (!addr || !*addr)
     {
	if (err)
	  *err = NSERR_NO_DATA;
	return NULL;
     }

   /* make sure we have memory available */
   if (!(ns = nsock_new(domain, type, 0, err)))
     return NULL;
   
   /* set the options */
   ns->opt = opts;
   ns->backlog = backlog;
   
   /* parse out the listen address */
   if (((domain == PF_INET
#ifdef INET6
	 || domain == PF_INET6
#endif
	 ) && nsock_inet_listen_parse(ns, addr) != NSERR_SUCCESS)
       || (domain == PF_UNIX && nsock_unix_listen_parse(ns, addr) != NSERR_SUCCESS))
     {
	if (err)
	  *err = ns->ns_errno;
	nsock_free(&ns);
	return NULL;
     }
   
   /* try to setup the socket */
   if (nsock_socket(ns) != NSERR_SUCCESS
       /* bind the address */
       || nsock_bind(ns) != NSERR_SUCCESS)
     {
	if (err)
	  *err = ns->ns_errno;
	nsock_free(&ns);
	return NULL;
     }
   
   /* start listening on the socket */
   if (type == SOCK_STREAM && nsock_listen(ns) != NSERR_SUCCESS)
     {
	if (err)
	  *err = ns->ns_errno;
	nsock_free(&ns);
	return NULL;
     }
   else if (type == SOCK_DGRAM)
     ns->state = NSS_SOCKET_LISTENING;
   
   return ns;
}


/*
 * parse the listen host/port
 */
static int
nsock_inet_listen_parse(ns, addr)
   nsock_t *ns;
   u_char *addr;
{
   int res_ret;
   
   /* resolve the listen addr */
   if (!(ns->inet_from = (u_char *)strdup((char *)addr)))
     return nsock_error(ns, NSERR_OUT_OF_MEMORY);
   
   /* resolve it */
   res_ret = nsock_inet_resolve_fwd(ns, ns->inet_from, &(ns->inet_fin));
   if (res_ret != NSERR_SUCCESS)
     return -1;
   
   /* resolving complete */
   return NSERR_SUCCESS;
}


/*
 * resolution of the to unix socket path for nsock_listen_init()
 */
static int
nsock_unix_listen_parse(ns, to)
   nsock_t *ns;
   u_char *to;
{
   int res_ret;
   
   /* resolve it */
   res_ret = nsock_unix_resolve(to, ns);
   if (res_ret != NSERR_SUCCESS)
     return nsock_error(ns, res_ret);
   
   /* destination must have a non-zero path */
   if (ns->unix_path[0] == 0)
     return nsock_error(ns, NSERR_UNIX_BAD_PATH);
   
   /* resolving complete */
   return NSERR_SUCCESS;
}
