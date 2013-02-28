/*
 * code to listen on a unix domain socket.
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>


#include "nsock_unix.h"
#include "nsock_errors.h"


#define NSOCK_UL_ERRS		7
nsockerror_t nsock_ul_errors[NSOCK_UL_ERRS] =
{
   { "Unknown error", 			0 },
   { "Invalid path", 			0 },
   { "Unable to chdir", 		1 },
   { "Unable to obtain a socket", 	1 },
   { "Unable to bind", 			1 },
   { "Unable to chmod", 		1 },
   { "Unable to listen", 		1 },
};


/* error definitions for code readability */
#define NSOCK_UL_BAD_PATH 	-1
#define NSOCK_UL_CHDIR_FAILED 	-2
#define NSOCK_UL_SOCKET_FAILED 	-3
#define NSOCK_UL_BIND_FAILED 	-4
#define NSOCK_UL_CHMOD_FAILED 	-5
#define NSOCK_UL_LISTEN_FAILED 	-6

#define NSOCK_UL_SUCCESS 	1


static int nsock_unix_listen_error(nsockunix_t *, int);


/*
 * libnsocks unix listen routine..
 * 
 * 1. break down the path
 * 2. chdir to the location of the socket (if path contains /)
 * 3. get a socket 
 * 4. try to bind the socket to the unix path
 * 5. try to chmod the socket to the specified mode
 * 6. try to listen on the socket with the specified backlog
 * 7. return the listening socket unless an error occurrs.
 * 7a. if an error occurrs return an negative index into
 *     the error structure array.
 */
int
nsock_unix_listen(nsu, bl)
   nsockunix_t *nsu;
   int bl;
{
   int sd;
   char *sn, path[1024];
   
   if (!nsu)
     return nsock_unix_listen_error(nsu, 0);
   /* break down the path */
   if (!nsu->path)
     return nsock_unix_listen_error(nsu, NSOCK_UL_BAD_PATH);
   strncpy(path, nsu->path, sizeof(path)-1);
   path[sizeof(path)-1] = '\0';
   
   /* try to split socket name and path */
   if ((sn = strrchr(path, '/')) != NULL)
     *sn++ = '\0';
   else
     sn = path;
   
   /* is it too long? */
   if (strlen(sn) > sizeof(nsu->sun.sun_path))
     return nsock_unix_listen_error(nsu, NSOCK_UL_BAD_PATH);

   /* can we chdir to the path we want it in? */
   if (sn != path 
       && chdir(path) == -1)
    return nsock_unix_listen_error(nsu, NSOCK_UL_CHDIR_FAILED);
   
   /* get the socket */
   if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
     return nsock_unix_listen_error(nsu, NSOCK_UL_SOCKET_FAILED);
   
   /* setup the struct for connecting */
   nsu->sun.sun_family = AF_UNIX;
   
   /* thought you found something, eh? */
   strcpy(nsu->sun.sun_path, sn);
   
#ifdef __44BSD__                                                                                                        
   nsu->sun.sun_len = strlen(sn);
#endif                                                                                                                  
   if (bind(sd, (struct sockaddr *)&nsu->sun, strlen(sn) + sizeof(nsu->sun.sun_family)
#ifdef __44BSD__                                                                                                        
	    + sizeof(nsu->sun.sun_len)
#endif                                                                                                                  
	    ) == -1)
     {
	close(sd);
	return nsock_unix_listen_error(nsu, NSOCK_UL_BIND_FAILED);
     }
   
   /* try to chmod it.. */
   if (!(nsu->opt & NSUNX_USE_UMASK)
       && nsu->mode > 0
       && chmod(sn, nsu->mode) == -1)
     {
	close(sd);
	return nsock_unix_listen_error(nsu, NSOCK_UL_CHMOD_FAILED);
     }
   
   /* listen! */
   if (listen(sd, bl) == -1)
     {
	close(sd);
	return nsock_unix_listen_error(nsu, NSOCK_UL_LISTEN_FAILED);
     }
   return sd;
}


/*
 * fill the error buf it is non-null..
 * 
 * return the passed in integer..
 */
static int
nsock_unix_listen_error(nsu, err)
   nsockunix_t *nsu;
   int err;
{
   if (!nsu)
     return err;
   return nsock_error(nsock_ul_errors, NSOCK_UL_ERRS,
		      nsu->ebuf, nsu->ebl,
		      err);
}

/*
 * initialize a malloc'd structure for listening..
 */
nsockunix_t *
nsock_unix_init_listen(on, opts, eb, ebl)
   u_char *on, *eb;
   u_long opts;
   int ebl;
{
   nsockunix_t *nsu;
   
   /* do we have something to listen on? */
   if (!on)
     return NULL;
   /* allocate memory */
   nsu = (nsockunix_t *)calloc(1, sizeof(nsockunix_t));
   if (!nsu)
     return NULL;
   /* dup the string */
   nsu->path = strdup(on);
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
