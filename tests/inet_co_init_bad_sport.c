/*
 * code to test libnsock:
 * connect init resolver failure
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
inet_co_init_bad_sport(void)
{
   nsock_t *nst;
   u_int ns_errno;
   
   /* print what we are testing.. */
   printf("%-32s", "inet_co_init_bad_sport");
   fflush(stdout);
   
   /* try to initialize the struct */
   ns_errno = NSERR_SUCCESS;
   nst = nsock_connect_init(PF_INET, SOCK_STREAM, TCP_HOST_INVALID_PORT, TCP_HOST_VALID, NSF_NONE, &ns_errno);
   if (ns_errno == NSERR_INET_CO_SRC_BAD_PORT)
     {
	printf("PASSED   nsock_connect_init failed: %s\n",
	       nsock_strerror_full_n(ns_errno));
        return;
     }
   else if (!nst)
     {
	printf("FAILED   nsock_connect_init failed: %s\n", 
	       nsock_strerror_full_n(ns_errno));
	_exit(1);
     }
   printf("FAILED   nsock_connect_init succeeded!\n");
   nsock_free(&nst);
   _exit(1);
}
