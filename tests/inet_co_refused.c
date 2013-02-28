/*
 * code to test libnsock:
 * inet connection refused
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
inet_co_refused(void)
{
   nsock_t *nst;
   u_int ns_errno;
   
   /* print what we are testing.. */
   printf("%-32s", "inet_co_refused");
   fflush(stdout);
   
   /* try to initialize the struct */
   if (!(nst = nsock_connect_init(PF_INET, SOCK_STREAM, NULL, TCP_HOST_LOCAL_UNUSED, NSF_NONE, &ns_errno)))
     {
	printf("FAILED   nsock_connect_init failed: %s\n", 
	       nsock_strerror_full_n(ns_errno));
	_exit(1);
     }
   
   /* try to connect */
   if (nsock_connect_out(nst) != NSERR_SUCCESS)
     {
	if (nst->ns_errno == NSERR_CONNECT_FAILED) 	/* XXX: hardcoded return value */
	  {
	     printf("PASSED   nsock_connect_out: %s\n", nsock_strerror_full(nst));
	     nsock_free(&nst);
	     return;
	  }
	/* wrong error? */
	printf("FAILED   nsock_connect_out: %s\n", nsock_strerror_full(nst));
	nsock_free(&nst);
	_exit(1);
     }
   
   /* connection established? */
   printf("FAILED   nsock_connect_out succeeded\n");
   nsock_free(&nst);
   _exit(1);
}
