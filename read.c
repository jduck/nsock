/*
 * a read() wrapper with a timeout
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <unistd.h>
#include <errno.h>


ssize_t
nsock_read(ns, buf, bl)
   nsock_t *ns;
   void *buf;
   size_t bl;
{
   ssize_t ret;
   
   /* if we have a timeout, set it up */
   if (ns->read_timeout > 0)
     if (nsock_read_alarm(ns) != NSERR_SUCCESS)
       return -1;
   
   /* read stuff */
#ifdef HAVE_SSL
   if (ns->opt & NSF_USE_SSL)
     ret = SSL_read(ns->ns_ssl.ssl, buf, bl);
   else
#endif
     ret = read(ns->sd, buf, bl);
   
   /* reset alarm if needed */
   if (ns->read_timeout > 0)
     if (nsock_alarm(ns, 0) != NSERR_SUCCESS)
       return -1;
   
   /* check for our alarm interruption and switch it to
    * timeout */
   if (ret == -1)
     {
	if (errno == EINTR)
	  errno = ETIMEDOUT;
	return nsock_error(ns, NSERR_READ_ERROR);
     }
   if (ret == 0)
     return nsock_error(ns, NSERR_READ_EOF);
   return ret;
}
