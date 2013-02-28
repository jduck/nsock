#include "nsock.h"
#include "errors.h"

#include "nsock_test.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

void
unix_resolve_sock_too_long(void)
{
   nsock_t *ns;
   
   
   /* print what we are testing.. */
   printf("%-32s", "unix_resolve_sock_too_long");
   fflush(stdout);
   
   ns = nsock_new(PF_UNIX, SOCK_STREAM, 0, NULL);
   if (!ns)
     {
	perror("FAILED  ");
	_exit(1);
     }
   
   if (nsock_unix_resolve(UNIX_SOCK_SOCK_TOO_LONG, ns) != NSERR_SUCCESS)
     {
	if (ns->ns_errno == NSERR_UNIX_SOCK_TOO_LONG)
	  printf("PASSED   %s\n", nsock_strerror_full(ns));
	else
	  printf("FAILED   %s\n", nsock_strerror_full(ns));
     }
   else if (strcmp(UNIX_PATH_SHORT, ns->unix_path) != 0)
     printf("FAILED   paths do not match\n");
   else if (strcmp(UNIX_SOCK_TOO_LONG, ns->unix_sun.sun_path) != 0)
     printf("FAILED   socket names do not match\n");
   else
     printf("FAILED   paths are correctly split\n");
   
   nsock_free(&ns);
}
