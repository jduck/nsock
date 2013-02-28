/*
 * small thing to concatenate a host and port for you!
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <resolv.h>


u_char *
nsock_tcp_host(host, port)
   u_char *host;
   int port;
{
   static u_char buf[MAXDNAME + 1 + 5 + 1]; 	/* enough for a fqdn and a :port */
   
   /* check port */
   if (port < 0 || port > USHRT_MAX)
     return NULL;
   /* host and port.. */
   if (host)
     snprintf(buf, sizeof(buf) - 1, "%s:%hu", host, (u_short)port);
   /* only port */
   else
     snprintf(buf, sizeof(buf) - 1, ":%hu", (u_short)port);
   buf[sizeof(buf)-1] = '\0';
   return buf;
}
