#include "nsock.h"
#include "errors.h"

#include <fcntl.h>


#ifdef NON_BLOCKING_CONNECTS
int
nsock_set_blocking(ns, status)
   nsock_t *ns;
   int status;
{
   int res, nonb = 0;

# if defined(NBLOCK_POSIX)
   nonb |= O_NONBLOCK;
# else
#  if defined(NBLOCK_BSD)
   nonb |= O_NDELAY;
#  else
#   if defined(NBLOCK_SYSV)
   res = 1;

   if (ioctl(ns->sd, FIONBIO, &res) < 0)
     return nsock_error(ns, NSERR_NONB_FAILED);

#   else
#    error no idea how to set an fds blocking status
#   endif /* NBLOCK_SYSV */
#  endif /* NBLOCK_BSD */
# endif /* NON_POSIX */
# if (defined(NBLOCK_POSIX) || defined(NBLOCK_BSD)) && !defined(NBLOCK_SYSV)
   if ((res = fcntl(ns->sd, F_GETFL, 0)) == -1)
     return nsock_error(ns, NSERR_NONB_FAILED);
   
   else if ((status == 0 && fcntl(ns->sd, F_SETFL, res | nonb) == -1)
	    || (status == 1 && fcntl(ns->sd, F_SETFL, res & ~nonb) == -1))
     return nsock_error(ns, NSERR_NONB_FAILED);
   
# endif /* (NBLOCK_POSIX || NBLOCK_BSD) && !NBLOCK_SYSV */
   return NSERR_SUCCESS;
}



/*
 * returns true or false depending on the connection status
 * on the specified socket
 */
int
nsock_is_connected(ns)
   nsock_t *ns;
{
   struct sockaddr_in sin;
   socklen_t sl = sizeof(sin);
   
   /* if getpeername succeeds we connected */
   if (getpeername(ns->sd, (struct sockaddr *)&sin, &sl) != -1)
     return 1;
   return 0;
}

#endif
