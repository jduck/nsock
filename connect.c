/*
 * a nice function for connecting a socket
 *
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <errno.h>


/*
 * connect a socket
 * 
 * the target/source should already be resolved before 
 * calling this
 */
int
nsock_connect(ns)
   nsock_t *ns;
{
   int cr;
   int old_errno;
   
   /* check out the struct passed in */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_OBTAINED) < 0)
     return -1;
   
   /* check for unsupported options */
#ifndef HAVE_SSL
   if (ns->opt & NSF_USE_SSL)
     return nsock_error(ns, NSERR_NO_SUPPORT);
#endif
#ifndef NON_BLOCKING_CONNECTS
   if (ns->opt & NSF_NON_BLOCK)
     return nsock_error(ns, NSERR_NO_SUPPORT);

#else   

   /* set non-blocking.. */
   if (ns->opt & NSF_NON_BLOCK)
     {
	if (nsock_set_blocking(ns, 0) != NSERR_SUCCESS)
	  {
	     nsock_close(ns);
	     return -1;
	  }
     }
   else
#endif
     if (ns->connect_timeout > 0)
       /* timeout after X seconds if we are blocking */
       if (nsock_connect_alarm(ns) != NSERR_SUCCESS)
	 return -1;
   
   /* try to connect the socket up */ 
   if (ns->domain == PF_INET)
     cr = connect(ns->sd, (struct sockaddr *)&(ns->inet_tin), sizeof(struct sockaddr_in));
#ifdef INET6
   else if (ns->domain == PF_INET6)
     cr = connect(ns->sd, (struct sockaddr *)&(ns->inet_tin), sizeof(struct sockaddr_in6));
#endif
   else if (ns->domain == PF_UNIX)
     cr = connect(ns->sd, (struct sockaddr *)&(ns->unix_sun), sizeof(ns->unix_sun));
   else
     {
	nsock_close(ns);
	return nsock_error(ns, NSERR_INVALID_DOMAIN);
     }
   old_errno = errno;
   
#ifdef NON_BLOCKING_CONNECTS
   if (!(ns->opt & NSF_NON_BLOCK))
#endif
     {
	if (ns->connect_timeout > 0)
	  {
	     if (nsock_alarm(ns, 0) != NSERR_SUCCESS)
	       {
		  nsock_close(ns);
		  return -1;
	       }
	  }
     }
#ifdef NON_BLOCKING_SOCKETS
   /* turn off non-blocking if we turned it on.. */
   else
     {
	if (nsock_set_blocking(ns, 1) != NSERR_SUCCESS)
	  return -1;
     }
#endif
#ifdef DEBUG_CONNECT
   fprintf(stderr, "nsock: connect() returned: %d\n", cr);
#endif
   
   /* see what connect(2) said */
   errno = old_errno;
   if (cr == -1)
     {
#ifdef NON_BLOCKING_CONNECTS
	/* see if we get an immediate connection from non-blocking socket */
	if (errno == EINPROGRESS)
	  {
	     int ntcr = nsock_is_connected(ns);
	     
	     ns->state = NSS_SOCKET_CONNECTING;
	     /* non-blocking connected quickly */
	     if (ntcr)
	       {
# ifdef DEBUG_CONNECT
		  fprintf(stderr, "nsock: connect() occurred right away!\n");
# endif
		  ns->state = NSS_SOCKET_CONNECTED;
# ifdef HAVE_SSL
		  if (ns->opt & NSF_USE_SSL)
		    return nsock_ssl_connect(ns);
# endif		  
		  return NSERR_SUCCESS;
	       }
	     else
	       return nsock_error(ns, NSERR_CONNECT_IN_PROGRESS);
	  }
	else
#endif	
	  if (errno == EINTR)
	    errno = ETIMEDOUT;
	nsock_close(ns);
	return nsock_error(ns, NSERR_CONNECT_FAILED);
     }
   /* blocking connect returned success */
   ns->state = NSS_SOCKET_CONNECTED;
# ifdef HAVE_SSL
   if (ns->opt & NSF_USE_SSL)
     return nsock_ssl_connect(ns);
# endif		  
   return NSERR_SUCCESS;
}
