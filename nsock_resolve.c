/*
 * a nice function for resolving host/host:port/:port
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "nsock_resolve.h"


/*
 * libnsocks resolver routine..
 *
 * accepts host, host:port or :port
 *
 * returns an error as distinguished in the header file.
 */
int
nsock_resolve(hp, sin)
   u_char *hp;
   struct sockaddr_in *sin;
{
   int i;
   u_char dp[512], *dpp;
   
   /* nothing to resolve or nowhere to store it? */
   if (!hp)
     return NSOCK_R_NO_DATA;
   if (!sin)
     return NSOCK_R_NO_STORE;
   strncpy(dp, hp, sizeof(dp)-1);
   dp[sizeof(dp)-1] = '\0';
   
   memset(sin, 0, sizeof(struct sockaddr_in));
   
   /* check for port */
   if ((dpp = strchr(dp, ':')))
     {
	*dpp++ = '\0';
	/* check port validity */
	i = atoi(dpp);
	if (i < 0 || i > USHRT_MAX)
	  return NSOCK_R_BAD_PORT;
	sin->sin_port = htons(i);
	sin->sin_family = AF_INET;
     }
   
   /* try ip first, then resolver */
   if (*dp)
     {
	if (inet_aton(dp, &(sin->sin_addr)) == 0)
	  {
	     struct hostent *h = gethostbyname(dp);
	     
	     if (!h)
	       return NSOCK_R_BAD_HOST;
	     memcpy(&(sin->sin_addr.s_addr),
		    h->h_addr, sizeof(sin->sin_addr.s_addr));
	     sin->sin_family = h->h_addrtype;
	  }
	sin->sin_family = AF_INET;
     }

   /* did we store something? */
   if (sin->sin_family != 0)
     return NSOCK_R_SUCCESS;
   return NSOCK_R_NOT_FOUND;
}
