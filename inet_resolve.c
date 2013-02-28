/*
 * a nice function for resolving host/host:port/:port
 *
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <arpa/inet.h>
#include <netdb.h>


static int nsock_inet_resolve_split(u_char *, u_char **, u_char **);

int getinfo_errno = 0;


/*
 * libnsocks resolver routine..
 *
 * accepts host, host:port or :port
 * - port can be specified as a service or as a number
 *
 * returns an error as distinguished in the header file.
 */
int
nsock_inet_resolve_fwd(ns, addrstr, sap)
   nsock_t *ns;
   u_char *addrstr;
   struct sockaddr_storage *sap;
{
   u_char *addrcopy, *portstr = NULL;
   int res_ret;
   
   /* nothing to resolve or nowhere to store it? */
   if (!ns || !addrstr)
     return nsock_error(ns, NSERR_NO_DATA);
   if (!sap)
     return nsock_error(ns, NSERR_NO_STORAGE);
   
   /* make a copy */
   if (!(addrcopy = strdup(addrstr)))
     return nsock_error(ns, NSERR_OUT_OF_MEMORY);
   
   /* see if we have a port */
   if ((res_ret = nsock_inet_resolve_split(addrcopy, &addrstr, &portstr)) != NSERR_SUCCESS)
     {
	free(addrcopy);
	return nsock_error(ns, res_ret);
     }
   
   /* recognize special cases for INADDRY_ANY */
   if (strcmp(addrstr, "0") == 0 
       || strcmp(addrstr, "any") == 0
       || *addrstr == '\0')
     addrstr = NULL;
   
   /* resolve host/port from names to numbers */
   if ((addrstr && *addrstr) || (portstr && *portstr))
     {
	socklen_t slen;
	struct addrinfo hints;
	struct addrinfo *res;
	int gai_ret;
	
	/* setup some hints based on the socket in use */
	memset(&hints, 0, sizeof(hints));
	if (ns)
	  {
	     hints.ai_socktype = ns->type;
	     if (ns->opt & NSF_USE_FAMILY_HINT)
	       hints.ai_family = ns->domain;
	  }
	
	/* try numeric first */
	hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
	gai_ret = getaddrinfo(addrstr, portstr, &hints, &res);
	if (gai_ret != 0)
	  {
	     /* try again with names :-/ */
	     hints.ai_flags = AI_PASSIVE;
	     gai_ret = getaddrinfo(addrstr, portstr, &hints, &res);
	  }
	
	/* failure :( */
	if (gai_ret != 0)
	  {
	     free(addrcopy);
	     getinfo_errno = gai_ret;
	     return nsock_error(ns, NSERR_INET_RESOLVE_BAD_HOST);
	  }
	
	/* got it! */
	slen = sizeof(struct sockaddr_storage);
	if (res->ai_addrlen < slen)
	  slen = res->ai_addrlen;
	memcpy(sap, res->ai_addr, slen);
	/* update the socket domain */
	ns->domain = sap->ss_family;
	freeaddrinfo(res);
	free(addrcopy);
	return NSERR_SUCCESS;
     }
   return nsock_error(ns, NSERR_INET_RESOLVE_NOT_FOUND);
}


int
nsock_inet_resolve_rev(ns, sap, addrstr, aslen)
   nsock_t *ns;
   struct sockaddr_storage *sap;
   u_char *addrstr;
   u_int aslen;
{
   u_char hostbuf[NI_MAXHOST], servbuf[NI_MAXSERV];
   u_char *hostbufp = hostbuf;
   int flags = 0;
   int gni_ret;
   socklen_t salen;
   
   /* nothing to resolve or nowhere to store it? */
   if (!addrstr || aslen < 1)
     return nsock_error(ns, NSERR_NO_STORAGE);
   if (!ns || !sap)
     return nsock_error(ns, NSERR_NO_DATA);
   memset(addrstr, 0, aslen);
   
   /* setup the address length */
   if (sap->ss_family == PF_INET)
     {
	if (ns && !(ns->opt & NSF_NO_REVERSE_NAME)
	    && ((struct sockaddr_in *)sap)->sin_addr.s_addr == INADDR_ANY)
	  {
	     strcpy(hostbuf, "any");
	     hostbufp = NULL;
	  }
	salen = sizeof(struct sockaddr_in);
     }
#ifdef INET6
   else if (sap->ss_family == PF_INET6)
     {
	if (ns && !(ns->opt & NSF_NO_REVERSE_NAME)
	    && memcmp(&((struct sockaddr_in6 *)sap)->sin6_addr, &in6addr_any, sizeof(in6addr_any)) == 0)
	  {
	     strcpy(hostbuf, "any6");
	     hostbufp = NULL;
	  }
	salen = sizeof(struct sockaddr_in6);
     }
#endif
   else
     salen = sizeof(*sap);
   
   /* setup the getnameinfo(3) flags */
   if (ns && (ns->opt & NSF_NO_REVERSE_NAME))
     {
	flags |= NI_NUMERICHOST;
	flags |= NI_NUMERICSERV;
     }
   if (ns && ns->type == SOCK_DGRAM)
     flags |= NI_DGRAM;
   
   /* use getnameinfo(3) to reverse the address */
   if (hostbufp)
     memset(hostbufp, 0, NI_MAXHOST);
   memset(servbuf, 0, sizeof(servbuf));
   gni_ret = getnameinfo((struct sockaddr *)sap, salen,
			 hostbufp, hostbufp ? NI_MAXHOST - 1 : 0,
			 servbuf, sizeof(servbuf) - 1,
			 flags);
   if (gni_ret != 0)
     {
	getinfo_errno = gni_ret;
	return nsock_error(ns, NSERR_INET_RESOLVE_BAD_HOST);
     }
   
   /* it resolved! return the stuff in the buffer */
   snprintf(addrstr, aslen - 1, "%s:%s", hostbuf, servbuf);
   return NSERR_SUCCESS;
}




static int
nsock_inet_resolve_split(addrstr, hoststrp, portstrp)
   u_char *addrstr;
   u_char **hoststrp;
   u_char **portstrp;
{
   u_char *p;
   
   p = *hoststrp = addrstr;
#ifdef INET6
   /* check for ipv6 [x:x:x:x]:port */
   if ((p = strchr(p, '[')))
     {
	p++;
	*hoststrp = p;
	if (!(p = strchr(p, ']')))
	  return NSERR_INET_RESOLVE_NOT_FOUND;
	*p++ = '\0';
     }
   else
     p = addrstr;
#endif
   
   /* see if we have a : */
   if ((p = strchr(p, ':')))
     {
	*p++ = '\0';
	*portstrp = p;
     }
   return NSERR_SUCCESS;
}
