/*
 * low-level code for connecting to a unix domain socket.
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


#include "nsock_unix.h"
#include "nsock_errors.h"


#define NSOCK_UC_ERRS		5
nsockerror_t nsock_uc_errors[NSOCK_UC_ERRS] =
{
   { "Unknown error", 			0 },
   { "Invalid path", 			0 },
   { "Unable to chdir", 		1 },
   { "Unable to obtain a socket", 	1 },
   { "Unable to connect", 		1 },
};


/* error definitions for code readability */
#define NSOCK_UC_BAD_PATH 	-1
#define NSOCK_UC_CHDIR_FAILED 	-2
#define NSOCK_UC_SOCKET_FAILED 	-3
#define NSOCK_UC_CONNECT_FAILED -4

#define NSOCK_UC_SUCCESS 	1


static int nsock_unix_connect_error(nsockunix_t *, int);


/*
 * libnsocks unix connection routine..
 * 
 * 1. break down the path
 * 2. chdir to the location of the socket (if path contains /)
 * 3. get a socket
 * 4. attempt to connect via the socket
 * 5. return the connected socket unless an error occurrs.
 * 5a. if an error occurrs return an negative index into
 *     the error structure array.
 */
int
nsock_unix_connect(nsu)
   nsockunix_t *nsu;
{
   int sd;
   u_char *sn, npath[1024];
   
   if (!nsu)
     return nsock_unix_connect_error(nsu, 0);
   /* break down the path */
   if (!nsu->path)
     return nsock_unix_connect_error(nsu, NSOCK_UC_BAD_PATH);
   strncpy(npath, nsu->path, sizeof(npath)-1);
   npath[sizeof(npath)-1] = '\0';
   
   /* try to split socket name and path */
   if ((sn = strrchr(npath, '/')) != NULL)
     *sn++ = '\0';
   else
     sn = npath;
   
   /* is it too long? */
   if (strlen(sn) > sizeof(nsu->sun.sun_path))
     return nsock_unix_connect_error(nsu, NSOCK_UC_BAD_PATH);

   /* can we chdir to the path we want it in? */
   if (sn != npath 
       && chdir(npath) == -1)
    return nsock_unix_connect_error(nsu, NSOCK_UC_CHDIR_FAILED);
   
   /* get the socket */
   if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
     return nsock_unix_connect_error(nsu, NSOCK_UC_SOCKET_FAILED);
   
   /* setup the struct for connecting */
   nsu->sun.sun_family = AF_UNIX;
   
   /* thought you found something, eh? */
   strcpy(nsu->sun.sun_path, sn);
   
   /* connect it! */
#ifdef __44BSD__
   nsu->sun.sun_len = strlen(sn);
#endif
   if (connect(sd, (struct sockaddr *)&nsu->sun, strlen(sn) + sizeof(nsu->sun.sun_family)
#ifdef __44BSD__
	       + sizeof(nsu->sun.sun_len)
#endif
	       ) == -1)
     {
	close(sd);
	return nsock_unix_connect_error(nsu, NSOCK_UC_CONNECT_FAILED);
     }
   return sd;
}


/*
 * fill the error buf it is non-null..
 * 
 * return the passed in integer..
 */
static int
nsock_unix_connect_error(nsu, err)
   nsockunix_t *nsu;
   int err;
{
   if (!nsu)
     return err;
   return nsock_error(nsock_uc_errors, NSOCK_UC_ERRS,
		      nsu->ebuf, nsu->ebl,
		      err);
}


/*
 * allocate a structure and initialize it..
 */
nsockunix_t *
nsock_unix_init_connect(to, opts, eb, ebl)
   u_char *to, *eb;
   u_long opts;
   int ebl;
{
   nsockunix_t *nsu;
   
   /* make sure we have a target.. */
   if (!to)
     return NULL;
   /* make sure we have memory available */
   nsu = (nsockunix_t *)calloc(1, sizeof(nsockunix_t));
   if (!nsu)
     return NULL;
   /* dup the strings */
   nsu->path = strdup(to);
   if (!nsu->path)
     {
	free(nsu);
	return NULL;
     }
   /* set the options */
   nsu->opt = opts;
   /* if we have error buffer stuff, set it */
   if (eb && ebl > 0)
     {
	nsu->ebuf = eb;
	nsu->ebl = ebl;
     }
   return nsu;
}
