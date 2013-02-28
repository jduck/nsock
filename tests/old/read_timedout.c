/*
 * code to test libnsock:
 * valid tcp connection and read timeout
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>

#include "nsock_test.h"
#include "nsock_tcp.h"


void
read_timedout(void)
{
   char eb[256], buf[256];
   int sd;
   nsocktcp_t *nst;
   ssize_t nr;
   
   /* print what we are testing.. */
   printf("%-32s", "read_timedout");
   fflush(stdout);
   
   /* try to initialize the struct */
   if (!(nst = nsock_tcp_init_connect(NULL, TCP_HOST_VALID,
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
	printf("FAILED   nsock_tcp_connect: %s\n", eb);
	_exit(1);
     }
   /* connection established? */
   nr = nsock_read(sd, buf, sizeof(buf), 1);
   close(sd);
   if (nr == -1)
     {
	if (errno == ETIMEDOUT)
	  {
	     printf("PASSED   nsock_read failed: %s\n", strerror(errno));
	     return;
	  }
	printf("FAILED   nsock_read failed: %s\n", strerror(errno));
     }
   else
     {
	if (nr == 0)
	  printf("FAILED   nsock_read got EOF\n");
	else
	  printf("FAILED   nsock_read got something :(\n");
     }
   _exit(1);
}
