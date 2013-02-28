/*
 * a read() wrapper with a timeout
 * this one will stop if it gets a newline..
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"

#include <unistd.h>
#include <errno.h>


ssize_t
nsock_readln(ns, buf, bl)
   nsock_t *ns;
   void *buf;
   size_t bl;
{
   ssize_t ret = 0, tret;
   void *vp = buf;
   
   /* loop until we reach our buffer length, or we get a newline/eof */
   while (ret < bl - 1)
     {
	/* if we have a timeout, set it up */
	if (ns->read_timeout > 0)
	  nsock_read_alarm(ns);
	
	/* read one */
#ifdef HAVE_SSL
	if (ns->opt & NSF_USE_SSL)
	  tret = SSL_read(ns->ns_ssl.ssl, vp, 1);
	else
#endif
	  tret = read(ns->sd, vp, 1);
	
	/* reset timeout */
	if (ns->read_timeout > 0)
	  nsock_alarm(ns, 0);
	
	/* error? */
	if (tret == -1)
	  {
	     if (errno == EINTR)
	       errno = ETIMEDOUT;
	     /* empty? */
	     if (ret)
	       return ret;
	     return -1;
	  }
	/* EOF */
	if (tret == 0)
	  {
	     /* empty? */
	     if (ret)
	       return ret;
	     return 0;
	  }
	/* got one! */
	ret++;
	/* check for newline */
	if (*(u_char *)vp == '\n')
	  return ret;
	/* goin for another */
	vp++;
     }
   /* buffer is full, return it.. */
   *(char *)(buf + bl - 1) = '\0';
   /* XXX: off-by-one
    * 
   ((u_char *)buf)[bl] = '\0';
    */
   return ret;
}
