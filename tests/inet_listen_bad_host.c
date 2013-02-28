/*
 * code to test libnsock:
 * inet listen invalid host
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <nsock/nsock.h>
#include <nsock/errors.h>

#include "nsock_test.h"


void
inet_listen_bad_host(void)
{
   nsock_t *nst;
   u_int ns_errno;
   
   /* print what we are testing.. */
   printf("%-32s", "inet_listen_bad_host");
   fflush(stdout);
   
   /* try to initialize the struct */
   nst = nsock_listen_init(PF_INET, SOCK_STREAM, TCP_HOST_INVALID, 1, NSF_NONE, &ns_errno);
   if (ns_errno == NSERR_INET_LISTEN_BAD_HOST)
     {
	printf("PASSED   nsock_listen_init: %s\n",
	       nsock_strerror_full_n(ns_errno));
	return;
     }
   else if (!nst)
     {
	printf("FAILED   nsock_listen_init: %s\n", 
	       nsock_strerror_full_n(ns_errno));
	_exit(1);
     }
   
   /* listening? */
   printf("FAILED   nsock_listen_init succeeded\n");
   nsock_free(&nst);
   _exit(1);
}
