
#include <fcntl.h>

#include "nsock.h"

#ifdef NON_BLOCKING_CONNECTS
int
nsock_tcp_set_blocking(fd, status)
   int fd, status;
{
   int res, nonb = 0;

#if defined(NBLOCK_POSIX)
   nonb |= O_NONBLOCK;
#else
#if defined(NBLOCK_BSD)
   nonb |= O_NDELAY;
#else
#if defined(NBLOCK_SYSV)
   res = 1;

   if (ioctl(fd, FIONBIO, &res) < 0)
      return -1;
#else
#error no idea how to set an fds blocking status
#endif /* NBLOCK_SYSV */
#endif /* NBLOCK_BSD */
#endif /* NON_POSIX */
#if (defined(NBLOCK_POSIX) || defined(NBLOCK_BSD)) && !defined(NBLOCK_SYSV)
   if ((res = fcntl(fd, F_GETFL, 0)) == -1)
      return -1;
   else if ((status == 0 && fcntl(fd, F_SETFL, res | nonb) == -1)
	    || (status == 1 && fcntl(fd, F_SETFL, res & ~nonb) == -1))
      return -1;
#endif /* (NBLOCK_POSIX || NBLOCK_BSD) && !NBLOCK_SYSV */
   return 0;
}
#endif
