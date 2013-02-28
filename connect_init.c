/*
 * a nice function for initializing a connection
 *
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <string.h>


static int nsock_inet_connect_parse(nsock_t *, u_char *, u_char *);
static int nsock_unix_connect_parse(nsock_t *, u_char *);


/*
 * allocate a structure and initialize it..
 * 
 * any error will have the ns_errno stored in err
 */
nsock_t *
nsock_connect_init(domain, type, from, to, opts, err)
   int domain, type;
   u_char *from, *to;
   u_long opts;
   u_int *err;
{
   nsock_t *ns;
   
   /* make sure we have a target.. at the very least */
   if (!to || !*to)
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
   
   /* parse the data based on tyhe domain */
   if (domain == PF_INET
#ifdef INET6
       || domain == PF_INET6
#endif
       )
     {
	/* parse out the from/to host */
	if (nsock_inet_connect_parse(ns, from, to) != NSERR_SUCCESS)
	  {
	     if (err)
	       *err = ns->ns_errno;
	     nsock_free(&ns);
	     return NULL;
	  }
     }
   else if (domain == PF_UNIX)
     {
	/* parse out the from/to host */
	if (nsock_unix_connect_parse(ns, to) != NSERR_SUCCESS)
	  {
	     if (err)
	       *err = ns->ns_errno;
	     nsock_free(&ns);
	     return NULL;
	  }
     }
   return ns;
}


/*
 * resolution of the from/to addresses for nsock_connect_init()
 */
static int
nsock_inet_connect_parse(ns, from, to)
   nsock_t *ns;
   u_char *from, *to;
{
   int res_ret;
   struct sockaddr_in *sinp;
#ifdef INET6
   struct sockaddr_in6 *sin6p;
#endif
   
   /* handle the from address =) */
   if (from && *from)
     {
	/* make a copy */
	if (!(ns->inet_from = (u_char *)strdup((char *)from)))
	  return nsock_error(ns, NSERR_OUT_OF_MEMORY);
	
	/* resolve it */
	res_ret = nsock_inet_resolve_fwd(ns, ns->inet_from, &(ns->inet_fin));
	if (res_ret != NSERR_SUCCESS)
	  {
	     /* transform errors from the resolver into errors for this function */
	     if (ns->ns_errno == NSERR_INET_RESOLVE_BAD_PORT)
	       return nsock_error(ns, NSERR_INET_CO_SRC_BAD_PORT);
	     
	     if (ns->ns_errno == NSERR_INET_RESOLVE_BAD_HOST)
	       return nsock_error(ns, NSERR_INET_CO_SRC_BAD_HOST);
	     
	     if (ns->ns_errno == NSERR_INET_RESOLVE_NOT_FOUND)
	       return nsock_error(ns, NSERR_INET_CO_SRC_UNKNOWN);
	     
	     /* does the type match the socket we have? */
	     if (ns->inet_fin.ss_family != ns->domain)
	       return nsock_error(ns, NSERR_INET_CO_SRC_BAD_DOMAIN);
	     
	     /* return what the resolver said */
	     return nsock_error(ns, res_ret);
	  }
     }
   
   /* ok, now check out the target */
   if (!(ns->inet_to = (u_char *)strdup((char *)to)))
     return nsock_error(ns, NSERR_OUT_OF_MEMORY);
   
   /* resolve it */
   res_ret = nsock_inet_resolve_fwd(ns, ns->inet_to, &(ns->inet_tin));
   if (res_ret != NSERR_SUCCESS)
     {
	/* transform errors from the resolver into errors for this function */
	if (ns->ns_errno == NSERR_INET_RESOLVE_BAD_PORT)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_PORT);
	
	if (ns->ns_errno == NSERR_INET_RESOLVE_BAD_HOST)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_HOST);
	
	if (ns->ns_errno == NSERR_INET_RESOLVE_NOT_FOUND)
	  return nsock_error(ns, NSERR_INET_CO_DST_UNKNOWN);
	
	/* does the type match the socket we have? */
	if (ns->inet_tin.ss_family != ns->domain)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_DOMAIN);
	
	/* return what the resolver said */
	return nsock_error(ns, res_ret);
     }
   /* destination must have a non-zero port and address */
#ifdef INET6
   if (ns->inet_tin.ss_family == PF_INET)
     {
#endif
	sinp = (struct sockaddr_in *)&ns->inet_tin;
	if (sinp->sin_port == 0)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_PORT);
	if (sinp->sin_addr.s_addr == 0)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_HOST);
#ifdef INET6
     }
   else if (ns->inet_tin.ss_family == PF_INET6)
     {
	struct in6_addr in6any = IN6ADDR_ANY_INIT;
	sin6p = (struct sockaddr_in6 *)&ns->inet_tin;
	if (sin6p->sin6_port == 0)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_PORT);
	if (memcmp(sin6p->sin6_addr.s6_addr, &in6any, sizeof(sin6p->sin6_addr.s6_addr)) == 0)
	  return nsock_error(ns, NSERR_INET_CO_DST_BAD_HOST);
     }
#endif     
   
   /* resolving complete */
   return NSERR_SUCCESS;
}


/*
 * resolution of the to unix socket path for nsock_connect_init()
 */
static int
nsock_unix_connect_parse(ns, to)
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
