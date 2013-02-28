/*
 * some nice alarm wrappers and a handler for our socket timeouts
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>


/* our old alarm handler */
static void (*old_handler)(int);
static void nsock_alarm_handler(int);
static u_int old_alarm = 0;

int
nsock_alarm(ns, timeout)
   nsock_t *ns;
   u_int timeout;
{
   struct sigaction nsa, osa;
   
   /* reset the timeout */
   if (timeout == 0)
     {
	nsa.sa_handler = old_handler;
	if (sigaction(SIGALRM, &nsa, NULL) == -1)
	  return nsock_error(ns, NSERR_SIGACTION_FAILED);
     }
   else
     {
	/* set the timeout */
	nsa.sa_handler = nsock_alarm_handler;
	if (sigaction(SIGALRM, &nsa, &osa) == -1)
	  return nsock_error(ns, NSERR_SIGACTION_FAILED);
	old_handler = osa.sa_handler;
     }
   old_alarm = alarm(timeout);
   return NSERR_SUCCESS;
}


/*
 * simple do-nothing alarm handler
 */
static void
nsock_alarm_handler(sig)
   int sig;
{
   /* do nothing */
}
