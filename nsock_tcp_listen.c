/*
 * nice way of listening on a tcp socket
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "nsock_tcp.h"
#include "nsock_resolve.h"
#include "nsock_errors.h"


#define NSOCK_TL_ERRS		9
nsockerror_t nsock_tl_errors[NSOCK_TL_ERRS] =
{
   { "No error", 			0 }, /* should not be used */
   { "Unknown error", 			0 },
   { "Invalid listen port",    		0 },
   { "Unable to obtain a socket", 	1 },
   { "Invalid listen host",    		0 },
   { "Unable to bind to address", 	1 },
   { "Unable to listen", 		1 },
   { "Unable to set socket options",   	1 },
   { "Unable to get socket name",   	1 },
};

/* error definitions for code readability */
#define NSOCK_TL_UNKNOWN_ERROR 		-1
#define NSOCK_TL_BAD_LPORT 		-2
#define NSOCK_TL_SOCKET_FAILED 		-3
#define NSOCK_TL_BAD_LHOST 		-4
#define NSOCK_TL_BIND_FAILED 		-5
#define NSOCK_TL_LISTEN_FAILED 		-6
#define NSOCK_TL_SOCKOPT_FAILED 	-7
#define NSOCK_TL_GSN_FAILED 		-8


static int nsock_tcp_listen_error(nsocktcp_t *, int);


/*
 * listen on a tcp port.
 * wait for a transmisson control protocol connection on a certain port.
 * you can specify which address you want to listen on (virtual hosts)
 *
 * from can be <name>:<port> or <ip>:<port> or :<port>
 * 
 * TODO:
 *    various proxies
 * 
 * returns the listening socket or an negative index into the error
 * structure.
 *
 * note: accept() is not handled here.. do it yourself..
 */
int
nsock_tcp_listen(nst, bl)
   nsocktcp_t *nst;
   int bl;
{
   int s;
   socklen_t finlen = sizeof(nst->fin);
   
   if (!nst)
     return nsock_tcp_listen_error(nst, 0);
   /* do we have a from or a family? */
   if (!nst->from && nst->fin.sin_family != AF_INET)
     return nsock_tcp_listen_error(nst, NSOCK_TL_BAD_LHOST);
   /* must we resolve? */
   if (nst->from && nst->fin.sin_family != AF_INET)
     {
	s = nsock_resolve(nst->from, &(nst->fin));
	if (s != NSOCK_R_SUCCESS)
	  {
	     if (s == NSOCK_R_BAD_PORT)
	       return nsock_tcp_listen_error(nst, NSOCK_TL_BAD_LPORT);
	     if (s == NSOCK_R_BAD_HOST)
	       return nsock_tcp_listen_error(nst, NSOCK_TL_BAD_LHOST);
	     return nsock_tcp_listen_error(nst, 0); /* unknown failure */
	  }
	
	/* stick in a random port even if one was specified already */
	if (nst->opt & NSTCP_RAND_SRC_PORT)
	  {
	     u_short rand_port;
	     
	     srandom(time(NULL));
	     rand_port = 1 + (u_short)(random() % USHRT_MAX);
	     if (rand_port < 1024)
	       rand_port += 1024;
	     nst->fin.sin_port = htons(rand_port);
	  }
     }
   
   /* get a socket */
   if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
     return nsock_tcp_listen_error(nst, NSOCK_TL_SOCKET_FAILED);
   
   /* set socket option if specified */
   if (nst->opt & NSTCP_REUSE_ADDR)
     {
	u_int optval = 1;
	
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	  {
	     close(s);
	     return nsock_tcp_listen_error(nst, NSOCK_TL_SOCKOPT_FAILED);
	  }
     }

   /* bind */
   if (bind(s, (struct sockaddr *)&(nst->fin), finlen) == -1)
     {
	close(s);
	return nsock_tcp_listen_error(nst, NSOCK_TL_BIND_FAILED);
     }
   
   /* get the socket name */
   if (getsockname(s, (struct sockaddr *)&(nst->fin), &finlen) == -1)
     {
	close(s);
	return nsock_tcp_listen_error(nst, NSOCK_TL_GSN_FAILED);
     }
   
   /* listen */
   if (listen(s, bl) == -1)
     {
	close(s);
	return nsock_tcp_listen_error(nst, NSOCK_TL_LISTEN_FAILED);
     }
   return s;
}


/*
 * fill the error buf it is non-null..
 * 
 * return the passed in integer..
 */
static int
nsock_tcp_listen_error(nst, err)
   nsocktcp_t *nst;
   int err;
{
   if (!nst)
     return err;
   return nsock_error(nsock_tl_errors, NSOCK_TL_ERRS,
			  nst->ebuf, nst->ebl,
			  err);
}


/*
 * initialize a malloc'd structure for listening..
 */
nsocktcp_t *
nsock_tcp_init_listen(on, opts, eb, ebl)
   u_char *on, *eb;
   u_long opts;
   int ebl;
{
   nsocktcp_t *nst;
   
   /* do we have something to listen on? */
   if (!on)
     return NULL;
   /* allocate memory */
   nst = (nsocktcp_t *)calloc(1, sizeof(nsocktcp_t));
   if (!nst)
     return NULL;
   /* dup the string */
   nst->from = strdup(on);
   if (!nst->from)
     {
	free(nst);
	return NULL;
     }
   /* set the options */
   nst->opt = opts;
   /* if we have error buffer stuff, set it */
   if (eb && ebl > 0)
     {
	nst->ebuf = eb;
	nst->ebl = ebl;
     }
   return nst;
}
