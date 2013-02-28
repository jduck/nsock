/*
 * code to test libnsock:
 * tcp listen host resolver failed
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include "nsock_test.h"
#include "nsock_tcp.h"


void
tcp_listen_bad_host(void)
{
   char eb[256];
   int sd;
   nsocktcp_t *nst;
   
   /* print what we are testing.. */
   printf("%-32s", "tcp_listen_bad_host");
   fflush(stdout);
   
   /* try to initialize the struct */
   if (!(nst = nsock_tcp_init_listen(TCP_HOST_INVALID,
				      NSTCP_NONE, eb, sizeof(eb))))
     {
	printf("FAILED   nsock_tcp_listen_init failed\n");
	_exit(1);
     }
   
   /* try to listen */
   sd = nsock_tcp_listen(nst, 1);
   nsock_tcp_free(&nst);
   if (sd < 1)
     {
	if (sd == -3) 	/* XXX: hardcoded return value */
	  {
	     printf("PASSED   nsock_tcp_listen: %s\n", eb);
	     return;
	  }
	/* wrong error? */
	printf("FAILED   nsock_tcp_listen: %s\n", eb);
	_exit(1);
     }
   
   /* listening? */
   close(sd);
   printf("FAILED   nsock_tcp_listen succeeded\n");
   _exit(1);
}
