/*
 * code to test libnsock:
 * tcp connect port check failure
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include "nsock_test.h"
#include "nsock_tcp.h"


void
tcp_connect_invalid_dest_port(void)
{
   char eb[256];
   int sd;
   nsocktcp_t *nst;
   
   /* print what we are testing.. */
   printf("%-32s", "tcp_connect_invalid_dest_port");
   fflush(stdout);
   
   /* try to initialize the struct */
   if (!(nst = nsock_tcp_init_connect(NULL, TCP_HOST_INVALID_PORT,
				      NSTCP_NONE, eb, sizeof(eb))))
     {
	printf("FAILED   nsock_tcp_connect_init failed\n");
	_exit(1);
     }
   
   /* try to connect */
   sd = nsock_tcp_connect(nst, 0);
   nsock_tcp_free(&nst);
   if (sd < 1)
     {
	if (sd == -1) 	/* XXX: hardcoded return value */
	  {
	     printf("PASSED   nsock_tcp_connect: %s\n", eb);
	     return;
	  }
	/* wrong error? */
	printf("FAILED   nsock_tcp_connect: %s\n", eb);
	_exit(1);
     }
   
   /* connection established? */
   close(sd);
   printf("FAILED   nsock_tcp_connect succeeded\n");
   _exit(1);
}
