/*
 * small thing to concatenate a host and port for you!
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>

#include <limits.h>


u_char *
nsock_inet_host(host, port)
   u_char *host;
   int port;
{
   static u_char buf[NI_MAXHOST + 1 + NI_MAXSERV + 1]; 	/* enough for a fqdn and a :port */
   
   /* check port */
   if (port < 0 || port > USHRT_MAX)
     return NULL;
   /* host and port.. */
   if (host)
     snprintf((char *)buf, sizeof(buf) - 1, "%s:%hu", host, (u_short)port);
   /* only port */
   else
     snprintf((char *)buf, sizeof(buf) - 1, ":%hu", (u_short)port);
   buf[sizeof(buf)-1] = '\0';
   return buf;
}


u_char *
nsock_inet_host_str(host, portstr)
   u_char *host;
   u_char *portstr;
{
   static u_char buf[NI_MAXHOST + 1 + NI_MAXSERV + 1]; 	/* enough for a fqdn and a :port */
   
   /* host and port.. */
   if (host && portstr)
     snprintf((char *)buf, sizeof(buf) - 1, "%s:%s", host, portstr);
   /* only port */
   else if (portstr)
     snprintf((char *)buf, sizeof(buf) - 1, ":%s", portstr);
   else if (host)
     strncpy((char *)buf, (char *)host, sizeof(buf) - 1);
   else
     return NULL;
   buf[sizeof(buf)-1] = '\0';
   return buf;
}



int
nsock_inet_host_has_port(addrstr)
   u_char *addrstr;
{
   u_char *p;
   
   p = addrstr;
#ifdef INET6
   /* check for ipv6 [x:x:x:x]:port */
   if ((p = (u_char *)strchr((char *)p, '[')))
     {
	p++;
	if (!(p = (u_char *)strchr((char *)p, ']')))
	  return 0;
	p++;
     }
   else
     p = addrstr;
#endif
   
   /* see if we have a : */
   if ((p = (u_char *)strchr((char *)p, ':')))
     return 1;
   return 0;
}
