/*
 * nsock error reporting routines..
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <netdb.h>


nsockerror_t nsock_errors[NSERR_COUNT] =
{
   { "No error", 			NSERRNO_NSOCK }, /* should never be used */
   { "Unknown error", 			NSERRNO_NSOCK }, /* should never be used */
   
   /* sanity check errors */
   { "Unsupported option", 		NSERRNO_NSOCK },
   { "No storage provided",    		NSERRNO_NSOCK },
   { "No data provided",    		NSERRNO_NSOCK },
   { "No socket obtained",     		NSERRNO_NSOCK },
   { "Invalid socket domain",      	NSERRNO_NSOCK },
   { "Invalid socket type",    		NSERRNO_NSOCK },
   { "Invalid state",      		NSERRNO_NSOCK },
   { "Allocation failed",      		NSERRNO_LIBC },
   
   /* errors for nsock_read() */
   { "Unable to read",    		NSERRNO_LIBC },
   { "EOF during read",      		NSERRNO_NSOCK },
   
   /* errors for nsock_write() */
   { "Unable to write",    		NSERRNO_LIBC },
   { "EOF during write",      		NSERRNO_NSOCK },
   
   /* errors for nsock_alarm() */
   { "sigaction failed",      		NSERRNO_LIBC },

   /* errors for nsock_socket() */
   { "Socket already obtained",        	NSERRNO_NSOCK },
   { "Unable to obtain socket",        	NSERRNO_LIBC },
   { "Unable to set socket options", 	NSERRNO_LIBC },

   /* errors for nsock_close() */
   { "Unable to close socket",        	NSERRNO_LIBC },
   
   /* errors for nsock_bind() */
   { "Unsupported socket domain",      	NSERRNO_NSOCK },
   { "Unable to bind", 			NSERRNO_LIBC },

   /* errors for nsock_unix_bind() */
   { "Invalid socket path name",      	NSERRNO_NSOCK },
   { "Unable to get current directory",	NSERRNO_LIBC },
   { "Unable to change directory", 	NSERRNO_LIBC },
   { "Unable to change socket mode", 	NSERRNO_LIBC },
   
   /* errors for nsock_inet_bind() */
   { "Invalid bind port",    		NSERRNO_NSOCK },
   { "Invalid bind host",    		NSERRNO_NSOCK },

   /* errors for nsock_listen() */
   { "Unable to listen", 	       	NSERRNO_LIBC },
   
   /* errors for nsock_accept() */
   { "Unable to accept", 	       	NSERRNO_LIBC },
   { "No connections available", 	NSERRNO_LIBC },
   
   /* errors for non-blocking stuff */
   { "Unable to set non-blocking", 	NSERRNO_LIBC },

   /* errors for nsock_*_connect() */
   { "Unable to connect", 		NSERRNO_LIBC },
   { "Connect in progress", 		NSERRNO_LIBC },

   /* errors for nsock_connect_inet() */
   { "Invalid source port",    		NSERRNO_NSOCK },
   { "Invalid source host",		NSERRNO_RESOLV },
   { "Source address family mismatch", 	NSERRNO_NSOCK },
   { "Source host unknown",    		NSERRNO_NSOCK },
   { "Invalid destination port",	NSERRNO_NSOCK },
   { "Invalid destination host",       	NSERRNO_RESOLV },
   { "Destination address family mismatch",
					NSERRNO_RESOLV },
   { "Destination host unknown",       	NSERRNO_NSOCK },
   
   /* errors for nsock_inet_listen() */
   { "Invalid listen port",    		NSERRNO_NSOCK },
   { "Invalid listen host",		NSERRNO_RESOLV },
   { "Listen host unknown",    		NSERRNO_NSOCK },
   
   /* errors for nsock_inet_resolve_*() */
   { "Unable to resolve host", 		NSERRNO_NSOCK },
   { "Invalid port",    		NSERRNO_NSOCK },
   { "Invalid host",    		NSERRNO_RESOLV },

   /* errors for nsock_unix_resolve() */
   { "Socket path too long",      	NSERRNO_NSOCK },
   { "Socket name too long",      	NSERRNO_NSOCK },
   { "Invalid socket path",      	NSERRNO_NSOCK },
   
   /* errors for nsock_io_pipe() */
   { "Unable to select",      		NSERRNO_LIBC },
   { "Socket exception",      		NSERRNO_NSOCK },
   
   /* errors for nsock_ssl_context_init */
   { "Unable to obtain new context",   	NSERRNO_NSOCK },
   { "Invalid certificate file",       	NSERRNO_NSOCK },
   { "Invalid private key file",       	NSERRNO_NSOCK },
   { "Cert and key don't match", 	NSERRNO_NSOCK },
   { "Unable to set cipher list", 	NSERRNO_SSL },

   /* errors for ssl library use */
   { "SSL_new failed",   		NSERRNO_NSOCK },
   { "SSL syscall error",       	NSERRNO_SSL },
   { "SSL library error",       	NSERRNO_SSL },
   { "Unknown SSL error", 		NSERRNO_NSOCK },
};


/*
 * return a pointer to the array entry for this error
 */
const char *
nsock_strerror_n(ns_errno)
   u_int ns_errno;
{
   if (ns_errno >= NSERR_COUNT)
     ns_errno = NSERR_UNKNOWN_ERROR;
   return nsock_errors[ns_errno].str;
}

const char *
nsock_strerror(ns)
   nsock_t *ns;
{
   return nsock_strerror_n(ns->ns_errno);
}


/*
 * return a pointer to a static buffer holding
 * the error along with its errno string if supplied
 */
char *
nsock_strerror_full_n(ns_errno)
   u_int ns_errno;
{
   static char ret_buf[1024 + 1];
   nsockerror_t *nse;
#ifdef HAVE_SSL
   u_long ssl_err;
#endif
   
   memset(ret_buf, 0, sizeof(ret_buf));
   if (ns_errno >= NSERR_COUNT)
     ns_errno = NSERR_UNKNOWN_ERROR;
   nse = &(nsock_errors[ns_errno]);
   switch (nse->has_errno)
     {
      case NSERRNO_NSOCK:
	strncpy(ret_buf, nse->str, sizeof(ret_buf) - 1);
	break;
	
      case NSERRNO_LIBC:
	snprintf(ret_buf, sizeof(ret_buf) - 1, "%s: %s", nse->str, strerror(errno));
	break;
	
      case NSERRNO_RESOLV:
	// snprintf(ret_buf, sizeof(ret_buf) - 1, "%s: %s", nse->str, hstrerror(h_errno));
	snprintf(ret_buf, sizeof(ret_buf) - 1, "%s: %s", nse->str, gai_strerror(getinfo_errno));
	break;
	
#ifdef HAVE_SSL	
      case NSERRNO_SSL:
	ssl_err = ERR_get_error();
	if (!ssl_err)
	  snprintf(ret_buf, sizeof(ret_buf) - 1, "%s: %s", nse->str, strerror(errno));
	else
	  snprintf(ret_buf, sizeof(ret_buf) - 1, "%s: %s", nse->str, ERR_error_string(ssl_err, NULL));
	break;
#endif

      default:
	strcpy(ret_buf, "No error string available");
	break;
     }
   return ret_buf;
}

char *
nsock_strerror_full(ns)
   nsock_t *ns;
{
   return nsock_strerror_full_n(ns->ns_errno);
}


/*
 * set the errno and return -1
 */
int
nsock_error(ns, err)
   nsock_t *ns;
   u_int err;
{
   if (ns)
     {
	ns->ns_errno = err;
	return -1;
     }
   return err;
}


/*
 * generic structure validation.
 */
int
nsock_generic_check(ns, req_domain, min_state)
   nsock_t *ns;
   int req_domain;
   u_long min_state;
{
   /* nothing provided? */
   if (!ns)
     {
	/* not supposed to print on stderr, but this is bad.. programmer should 
	 * notice this in debugging ;) */
	fprintf(stderr, "%s: %s\n", __func__, nsock_errors[NSERR_NO_STORAGE].str);
	return -1;
     }
   
   /* state too low? */
   if (ns->state < min_state)
     {
	ns->ns_errno = NSERR_INVALID_STATE;
	return -1;
     }

   /* a socket is required? */
   if (min_state >= NSS_SOCKET_OBTAINED
       && (ns->domain == PF_UNSPEC || ns->type == 0 
	   || ns->sd == -1))
     {
	ns->ns_errno = NSERR_NO_SOCKET;
	return -1;
     }

   /* invalid domain? */
   if (req_domain != PF_UNSPEC && ns->domain != req_domain)
     {
	ns->ns_errno = NSERR_INVALID_DOMAIN;
	return -1;
     }
   return NSERR_SUCCESS;
}
