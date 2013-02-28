/*
 * a read() wrapper with a timeout
 * this one will stop if it gets a newline..
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
nsock_readln(sd, buf, bl, timeout)
   int sd;
   void *buf;
   size_t bl;
   u_int timeout;
{
   ssize_t ret = 0, tret;
   void *vp = buf;
   
   /* sanitize timeout */
   if (timeout < 0)
     timeout = 0;
   /* loop until we reach our buffer length, or we get a newline/eof */
   while (ret < bl - 1)
     {
	/* if we have a timeout, set it up */
	if (timeout > 0)
	  {
#ifndef __sun__
	     siginterrupt(SIGALRM, 1);
#endif
	     old_alarm_handler = signal(SIGALRM, handle_alarm);
	     alarm(timeout);
	  }
	tret = read(sd, vp, 1);
	if (timeout > 0)
	  {
	     alarm(0);
	     signal(SIGALRM, old_alarm_handler);
	  }
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
