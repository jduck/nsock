/*
 * nsock_accept()
 * 
 * accept an incoming connection!
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <errno.h>


int
nsock_accept(ns, cns)
   nsock_t *ns, *cns;
{
   int accept_ret;
   socklen_t len;
   int old_errno;
   
   
   /* check our settings in the structure */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_LISTENING) != NSERR_SUCCESS)
     return -1;
   
   /* make sure we have storage */
   if (!cns)
     return nsock_error(ns, NSERR_NO_STORAGE);
   
   /* check for unsupported options turned on */
#ifndef HAVE_SSL
   if (ns->opt & NSF_USE_SSL)
     return nsock_error(ns, NSERR_NO_SUPPORT);
#endif
#ifndef NON_BLOCKING_CONNECTS
   if (ns->opt & NSF_NON_BLOCK)
     return nsock_error(ns, NSERR_NO_SUPPORT);
   
#else   
   
   /* set non-blocking if requested.. */
   if (ns->opt & NSF_NON_BLOCK)
     {
	if (nsock_set_blocking(ns, 0) != NSERR_SUCCESS)
	  return -1;
     }
   else
#endif
     if (ns->connect_timeout > 0)
       /* timeout after X seconds if we are blocking */
       if (nsock_connect_alarm(ns) != NSERR_SUCCESS)
	 return -1;
   
   
   /* is it right? (either UNIX or INET are allowed here) */
   if (ns->domain == PF_INET)
     {
	len = sizeof(struct sockaddr_in);
	accept_ret = accept(ns->sd, (struct sockaddr *)&(cns->inet_fin), &len);
     }
#ifdef INET6
   else if (ns->domain == PF_INET6)
     {
	len = sizeof(struct sockaddr_in6);
	accept_ret = accept(ns->sd, (struct sockaddr *)&(cns->inet_fin), &len);
     }
#endif
   else if (ns->domain == PF_UNIX)
     {
	len = sizeof(cns->unix_sun);
	accept_ret = accept(ns->sd, (struct sockaddr *)&(cns->unix_sun), &len);
     }
   else
     return nsock_error(ns, NSERR_INVALID_DOMAIN);
   old_errno = errno;
   
   /* reset alarm timeout if NOT non-blocking and timeout exists */
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
   
   errno = old_errno;
   if (accept_ret == -1)
     {
#ifdef NON_BLOCKING_CONNECTS
	if (errno == EAGAIN || errno == EWOULDBLOCK)
	  return nsock_error(ns, NSERR_ACCEPT_NONE_AVAIL);
	else
#endif
	  if (errno == EINTR)
	    errno = ETIMEDOUT;
	return nsock_error(ns, NSERR_ACCEPT_FAILED);
     }
   
   /* accept success! */
   cns->sd = accept_ret;
   cns->state = NSS_SOCKET_CONNECTED;
#ifdef HAVE_SSL
   if (cns->opt & NSF_USE_SSL)
     {
	if (nsock_ssl_accept(cns) != NSERR_SUCCESS)
	  return nsock_error(ns, cns->ns_errno);
     }
#endif
   return NSERR_SUCCESS;
}
