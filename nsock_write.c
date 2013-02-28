/*
 * a write() wrapper with a timeout
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock_tcp.h"

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

static void handle_alarm(int);
static void (*old_alarm_handler)(int);


ssize_t
nsock_write(sd, buf, bl, timeout)
   int sd;
   void *buf;
   size_t bl;
   u_int timeout;
{
   ssize_t ret;
   
   /* sanitize timeout */
   if (timeout < 0)
     timeout = 0;
   /* if we have a timeout, set it up */
   if (timeout > 0)
     {
#ifndef __sun__
	siginterrupt(SIGALRM, 1);
#endif
	old_alarm_handler = signal(SIGALRM, handle_alarm);
	alarm(timeout);
     }
   ret = write(sd, buf, bl);
   if (timeout > 0)
     {
	alarm(0);
	signal(SIGALRM, old_alarm_handler);
     }
   if (ret == -1 && errno == EINTR)
     errno = ETIMEDOUT;
   return ret;
}


/*
 * just a little alarm signal handler..
 */
static void
handle_alarm(int sig)
{
   /* do nothing */
}
