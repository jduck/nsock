/*
 * a nice function for connecting via tcp sock_stream
 *
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "nsock_tcp.h"
#include "nsock_resolve.h"
#include "nsock_errors.h"


#define NSOCK_TC_ERRS 		13
nsockerror_t nsock_tc_errors[NSOCK_TC_ERRS] =
{
   { "No error", 			0 }, /* should never be used */
   { "Unknown error", 			0 },
   { "Invalid target port",    		0 },
   { "Invalid source port",    		0 },
   { "Unable to obtain a socket", 	1 },
   { "Invalid target host",    		0 },
   { "Invalid source host", 		0 },
   { "Unable to bind to address", 	1 },
   { "Unable to connect", 		1 },
   { "Unable to set non-blocking", 	1 },
   { "Unsupported option", 		0 },
   { "Unable to set socket options",  	1 },
   { "Unable to get socket name",      	1 },
};


/* error definitions for code readability */
#define NSOCK_TC_UNKNOWN_ERROR 		-1
#define NSOCK_TC_BAD_DPORT 		-2
#define NSOCK_TC_BAD_SPORT 		-3
#define NSOCK_TC_SOCKET_FAILED 		-4
#define NSOCK_TC_BAD_DHOST 		-5
#define NSOCK_TC_BAD_SHOST 		-6
#define NSOCK_TC_BIND_FAILED 		-7
#define NSOCK_TC_CONNECT_FAILED 	-8
#define NSOCK_TC_NONB_FAILED 		-9
#define NSOCK_TC_NO_SUPPORT 		-10
#define NSOCK_TC_SOCKOPT_FAILED		-11
#define NSOCK_TC_GSN_FAILED 		-12

#define NSOCK_TC_SUCCESS 	1


static void handle_alarm(int);
static void (*old_alarm_handler)(int);

static int nsock_tcp_connect_parse(nsocktcp_t *);
static int nsock_tcp_connect_error(nsocktcp_t *, int);


/*
 * libnsocks tcp connection routine..
 * 
 * 1. if needed, resolves hosts
 * 2. attempt to connect..
 * 3. return the connected socket (or attempting to connect)
 *    unless an error occurrs.
 * 3a. if an error occurrs return an negative index into
 *     the error structure array.
 * 
 * NOTE: if non-blocking sockets are on, errno will be set 
 * to EINPROGRESS..  The calling code must handle this.
 */
int
nsock_tcp_connect(nst, timeout)
   nsocktcp_t *nst;
   int timeout;
{
   int s, cr;
   socklen_t finlen = sizeof(nst->fin);
   
   if (!nst)
     return nsock_tcp_connect_error(nst, 0);
   /* we must have at least to or tin.sin_family */
   if (!nst->to && nst->tin.sin_family != AF_INET)
     return nsock_tcp_connect_error(nst, NSOCK_TC_BAD_DHOST);
   /* must we resolve? */
   if ((nst->from && nst->fin.sin_family != AF_INET)
       || (nst->to && nst->tin.sin_family != AF_INET))
     {
	s = nsock_tcp_connect_parse(nst);
	if (s != NSOCK_TC_SUCCESS)
	  return nsock_tcp_connect_error(nst, s);
     }
   
   /* check for unsupported options */
#ifndef NON_BLOCKING_CONNECTS
   if (nst->opt & NSTCP_NON_BLOCK)
     return nsock_tcp_connect_error(nst, NSOCK_TC_NO_SUPPORT);
#endif
   
   /* try for a tcp socket.. */
   if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
     return nsock_tcp_connect_error(nst, NSOCK_TC_SOCKET_FAILED);

   /* set socket option if specified */
   if (nst->opt & NSTCP_REUSE_ADDR)
     {
	u_int optval = 1;
	
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	  {
	     close(s);
	     return nsock_tcp_connect_error(nst, NSOCK_TC_SOCKOPT_FAILED);
	  }
     }
   
   if (nst->fin.sin_family == AF_INET
       || nst->opt & NSTCP_RAND_SRC_PORT)
     {
	/* stick in a random port even if one was specified already */
	if (nst->opt & NSTCP_RAND_SRC_PORT)
	  {
	     u_short rand_port;
	     
	     srandom(time(NULL));
	     rand_port = 1 + (u_short)(random() % USHRT_MAX);
	     if (rand_port < 1024)
	       rand_port += 1024;
	     nst->fin.sin_port = htons(rand_port);
	     nst->fin.sin_family = AF_INET;
	  }
	
	/* try to bind.. */
	if (bind(s, (struct sockaddr *)&(nst->fin), finlen) == -1)
	  {
	     close(s);
	     return nsock_tcp_connect_error(nst, NSOCK_TC_BIND_FAILED);
	  }
	
	/* if we have a zero port, find out what got used */
	if (getsockname(s, (struct sockaddr *)&(nst->fin), &finlen) == -1)
	  {
	     close(s);
	     return nsock_tcp_connect_error(nst, NSOCK_TC_GSN_FAILED);
	  }
     }
   
#ifdef NON_BLOCKING_CONNECTS
   /* set non-blocking.. */
   if (nst->opt & NSTCP_NON_BLOCK)
     {
	if (nsock_tcp_set_blocking(s, 0) < 0)
	  {
	     close(s);
	     return nsock_tcp_connect_error(nst, NSOCK_TC_NONB_FAILED);
	  }
     }
   else
#endif
     {
	if (timeout > 0)
	  {
	     /* timeout after X seconds if we are blocking */
#ifndef __sun__
	     siginterrupt(SIGALRM, 1);
#endif
	     old_alarm_handler = signal(SIGALRM, handle_alarm);
	     alarm(timeout);
	  }
     }
   cr = connect(s, (struct sockaddr *)&(nst->tin), sizeof(nst->tin));
#ifdef NON_BLOCKING_CONNECTS
   if (!(nst->opt & NSTCP_NON_BLOCK))
#endif
     {
	if (timeout > 0)
	  {
	     alarm(0);
	     signal(SIGALRM, old_alarm_handler);
	  }
     }
#ifdef DEBUG_CONNECT
   fprintf(stderr, "nsock: connect() returned: %d\n", cr);
#endif
   if (cr == -1)
     {
#ifdef NON_BLOCKING_CONNECTS
	if (errno != EINPROGRESS)
	  {
#endif
	     if (errno == EINTR)
	       errno = ETIMEDOUT;
	     close(s);
	     return nsock_tcp_connect_error(nst, NSOCK_TC_CONNECT_FAILED);
#ifdef NON_BLOCKING_CONNECTS
	  }
	else 
	  {
	     /* see if we got an immediate connection.. */
	     int ntcr = nsock_tcp_connected(s);
	     if (ntcr)
	       {
#ifdef DEBUG_CONNECT
		  fprintf(stderr, "nsock: connect() occurred right away!\n");
#endif
		  if (ntcr == 1)
		    return s;
		  close(s);
		  return nsock_tcp_connect_error(nst, NSOCK_TC_CONNECT_FAILED);
	       }
	  }
#endif
     }
   return s;
}


/*
 * this routine will parse the old string formats into
 * a usable format (sockaddr_in) and store them into the
 * structure.  it will return 1 or a negative index into
 * nsock_tc_errs upon error.
 *
 * ----old comments----
 * connect to a remote host/port using a stream from 'from'
 * aka transmission control protocol w/virtual hosts
 * 
 * host can be <name>:<port> or <ip>:<port>
 * from can be <name>:<port> or <ip>:<port> or :<port>
 * 
 * start the remote host with = if you want to block even 
 * if non-blocking is availabie...
 * 
 * TODO:
 *   source routing (@), various proxies (#,!)...
 * 
 */
static int
nsock_tcp_connect_parse(nst)
   nsocktcp_t *nst;
{
   char *dp;
   int i;
   
   if (!nst)
     return -1;
   
   /* copy the target if we have one */
   if (nst->tin.sin_family != AF_INET)
     {
	if (!(dp = nst->to))
	  return NSOCK_TC_BAD_DHOST;
	if (!*dp)
	  return NSOCK_TC_BAD_DHOST;
	/* do we have a blocking request? */
	if (*dp == '=')
	  {
	     dp++;
	     nst->opt &= ~NSTCP_NON_BLOCK;
	  }
	
	/* resolve the target... */
	i = nsock_resolve(dp, &(nst->tin));
	if (i != NSOCK_R_SUCCESS)
	  {
	     if (i == NSOCK_R_BAD_PORT)
	       return NSOCK_TC_BAD_DPORT;
	     if (i == NSOCK_R_BAD_HOST)
	       return NSOCK_TC_BAD_DHOST;
	     return NSOCK_TC_UNKNOWN_ERROR; /* unknown failure */
	  }
	if (nst->tin.sin_port == 0)
	  return NSOCK_TC_BAD_DPORT;
	if (nst->tin.sin_addr.s_addr == 0)
	  return NSOCK_TC_BAD_DHOST;
     }

   /* resolve the source */
   if (nst->fin.sin_family != AF_INET
       && nst->from && *(nst->from))
     {
	dp = nst->from;
	i = nsock_resolve(dp, &(nst->fin));
	if (i != NSOCK_R_SUCCESS)
	  {
	     if (i == NSOCK_R_BAD_PORT)
	       return NSOCK_TC_BAD_SPORT;
	     if (i == NSOCK_R_BAD_HOST)
	       return NSOCK_TC_BAD_SHOST;
	     return NSOCK_TC_UNKNOWN_ERROR; /* unknown failure */
	  }
     }
   return NSOCK_TC_SUCCESS;
}


/*
 * just a little alarm signal handler..
 */
static void
handle_alarm(int sig)
{
   /* do nothing
    * 
   alarm(0);
   signal(SIGALRM, old_alarm_handler);
    */
}


/*
 * fill the error buf it is non-null..
 * 
 * return the passed in integer..
 */
static int
nsock_tcp_connect_error(nst, err)
   nsocktcp_t *nst;
   int err;
{
   if (!nst)
     return err;
   return nsock_error(nsock_tc_errors, NSOCK_TC_ERRS,
			  nst->ebuf, nst->ebl,
			  err);
}


#ifdef NON_BLOCKING_CONNECTS
/*
 * returns true or false depending on the connection status
 * on the specified socket descriptor
 */
int
nsock_tcp_connected(sd)
   int sd;
{
   struct sockaddr_in sin;
   socklen_t sl = sizeof(sin);
   fd_set rd;
   struct timeval tv;

   /* if getpeername succeeds we connected */
   if (getpeername(sd, (struct sockaddr *)&sin, &sl) != -1)
     return 1;
   /* try to see if there is an error */
   FD_ZERO(&rd);
   FD_SET(sd, &rd);
   tv.tv_sec = 0;
   tv.tv_usec = 1;
   if (select(sd+1, &rd, NULL, NULL, &tv)
       && (FD_ISSET(sd, &rd)))
     {
	char tb;
	
	if (read(sd, &tb, 1) == -1)
	  return -1;
     }
   return 0;
}
#endif


/*
 * allocate a structure and initialize it..
 */
nsocktcp_t *
nsock_tcp_init_connect(from, to, opts, eb, ebl)
   u_char *from, *to, *eb;
   u_long opts;
   int ebl;
{
   nsocktcp_t *nst;
   
   /* make sure we have a target.. */
   if (!to)
     return NULL;
   /* make sure we have memory available */
   nst = (nsocktcp_t *)calloc(1, sizeof(nsocktcp_t));
   if (!nst)
     return NULL;
   /* dup the strings */
   if (from)
     {
	nst->from = strdup(from);
	if (!nst->from)
	  {
	     free(nst);
	     return NULL;
	  }
     }
   nst->to = strdup(to);
   if (!nst->to)
     {
	free(nst->from);
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
