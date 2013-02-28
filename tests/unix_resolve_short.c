#include "nsock.h"
#include "errors.h"

#include "nsock_test.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>


void
unix_resolve_short(void)
{
   nsock_t *ns;
   
   
   /* print what we are testing.. */
   printf("%-32s", "unix_resolve_short");
   fflush(stdout);
   
   ns = nsock_new(PF_UNIX, SOCK_STREAM, 0, NULL);
   if (!ns)
     {
	perror("FAILED  ");
	_exit(1);
     }
   
   if (nsock_unix_resolve(UNIX_SOCK_SHORT, ns) != NSERR_SUCCESS)
     printf("FAILED   %s\n", nsock_strerror_full(ns));
   else if (strcmp(UNIX_SOCK_SHORT, ns->unix_sun.sun_path) == 0)
     printf("PASSED   paths are identical\n");
   else
     printf("FAILED   paths do not match\n");
   
   nsock_free(&ns);
}
