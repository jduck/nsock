/*
 * code to test libnsock:
 * valid tcp connection and read some stuff
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
read_valid(void)
{
   char eb[256], buf[256];
   int sd;
   nsocktcp_t *nst;
   ssize_t nr;
   
   /* print what we are testing.. */
   printf("%-32s", "read_valid");
   fflush(stdout);
   
   /* try to initialize the struct */
   if (!(nst = nsock_tcp_init_connect(NULL, TCP_HOST_BANNER,
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
   nr = nsock_read(sd, buf, sizeof(buf), 5 /* timeout */);
   close(sd);
   if (nr == -1)
     printf("FAILED   nsock_read failed: %s\n", strerror(errno));
   else if (nr == 0)
     printf("FAILED   nsock_read returned EOF\n");
   else
     {
	char *p = buf;
	
	while (*p && (p - buf) < nr)
	  {
	     if (!isprint(*p))
	       *p = '.';
	     p++;
	  }
	*p = '\0';
	printf("PASSED   nsock_read got %d bytes: %s\n", nr, buf);
	return;
     }
   _exit(1);
}
