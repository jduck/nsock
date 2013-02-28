/*
 * nsock_bind()
 * 
 * bind a socket!
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>


/*
 * bind an inet address to a socket
 * 
 * the address must be resoved in ns->inet_fin or the ns->unix_sun
 * already upon calling this
 */
int
nsock_bind(ns)
   nsock_t *ns;
{
   int bind_ret;
   socklen_t len;
   u_char full_path[PATH_MAX + 1];
   
   /* check our settings in the structure */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_OBTAINED))
     return -1;

   /* use the domain's handler */
   if (ns->domain == PF_INET)
     {
	struct sockaddr_in *sinp;
	
	sinp = (struct sockaddr_in *)&ns->inet_fin;
	/* we must be already resolved. */
	if (sinp->sin_family != PF_INET)
	  return nsock_error(ns, NSERR_INET_BIND_NO_ADDR);
     
	/* stick in a random port even if one was specified already */
	if (ns->opt & NSF_RAND_SRC_PORT)
	  {
	     u_short rand_port;
	     
	     /* make sure we are seeded */
	     if (!nsock_initialized)
	       nsock_init();
	     rand_port = 1 + (u_short)(random() % USHRT_MAX);
	     if (rand_port < 1024)
	       rand_port += 1024;
	     sinp->sin_port = htons(rand_port);
	  }
	
	/* finally try to bind it */
	len = sizeof(struct sockaddr_in);
	bind_ret = bind(ns->sd, (struct sockaddr *)sinp, len);
     }

#ifdef INET6
   else if (ns->domain == PF_INET6)
     {
	struct sockaddr_in6 *sin6p;
	
	sin6p = (struct sockaddr_in6 *)&ns->inet_fin;
	/* we must be already resolved. */
	if (sin6p->sin6_family != PF_INET6)
	  return nsock_error(ns, NSERR_INET_BIND_NO_ADDR);
     
	/* stick in a random port even if one was specified already */
	if (ns->opt & NSF_RAND_SRC_PORT)
	  {
	     u_short rand_port;
	     
	     /* make sure we are seeded */
	     if (!nsock_initialized)
	       nsock_init();
	     rand_port = 1 + (u_short)(random() % USHRT_MAX);
	     if (rand_port < 1024)
	       rand_port += 1024;
	     sin6p->sin6_port = htons(rand_port);
	  }
	
	/* finally try to bind it */
	len = sizeof(struct sockaddr_in6);
	bind_ret = bind(ns->sd, (struct sockaddr *)sin6p, len);
     }
#endif
   
   
   else if (ns->domain == PF_UNIX)
     {
	/* do we have a path resolved? */
	if (!ns->unix_sun.sun_path[0])
	  {
	     nsock_close(ns);
	     return nsock_error(ns, NSERR_UNIX_BAD_PATH);
	  }
	
	/* can we chdir to the path we want it in? */
	if (ns->unix_path)
	  {
	     /* remember where we are now */
	     if (!getcwd((char *)full_path, sizeof(full_path)))
	       {
		  nsock_close(ns);
		  return nsock_error(ns, NSERR_UNIX_BIND_GETCWD_FAILED);
	       }
	     
	     /* go to where the socket is */
	     if (chdir((char *)ns->unix_path) == -1)
	       {
		  nsock_close(ns);
		  return nsock_error(ns, NSERR_UNIX_BIND_CHDIR_FAILED);
	       }
	  }
	
	/* finally try to bind it */
	len = sizeof(ns->unix_sun);
	bind_ret = bind(ns->sd, (struct sockaddr *)&(ns->unix_sun), len);
	
	if (bind_ret != -1)
	  {
	     /* change the mode if necessary */
	     if (!(ns->opt & NSF_UNIX_USE_UMASK)
		 && ns->unix_mode > 0
		 && chmod(ns->unix_sun.sun_path, ns->unix_mode) == -1)
	       {
		  nsock_close(ns);
		  return nsock_error(ns, NSERR_UNIX_BIND_CHMOD_FAILED);
	       }
	     
	     /* change back if needed */
	     if (ns->unix_path)
	       {
		  /* go to where the socket is */
		  if (chdir((char *)full_path) == -1)
		    {
		       nsock_close(ns);
		       return nsock_error(ns, NSERR_UNIX_BIND_CHDIR_FAILED);
		    }
	       }
	  }
     }
   
   
   else
     {
	nsock_close(ns);
	return nsock_error(ns, NSERR_BIND_UNSUP_DOMAIN);
     }
   

   /* bind error? */
   if (bind_ret == -1)
     {
	nsock_close(ns);
	return nsock_error(ns, NSERR_BIND_FAILED);
     }
   ns->state = NSS_SOCKET_BOUND;
   return NSERR_SUCCESS;
}
