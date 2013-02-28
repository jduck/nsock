#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <nsock/nsock.h>
#include <nsock/errors.h>

#include <netdb.h>

#include "nsock_test.h"


int
main(int c, char **v)
{
   nsock_t *ns;
   int res_ret;
   char addr[NI_MAXHOST + 1 + NI_MAXSERV + 1];
   socklen_t addrlen;
   u_int ns_errno;
   struct sockaddr_in6 *sin6p;
   struct sockaddr_in *sinp;
   void *addr_p;
   in_port_t port;
   
   if (c < 2)
     {
	printf("usage: %s <host>[:<port>]\n", v[0]);
	return 1;
     }
   
   if (!(ns = nsock_new(PF_INET6, SOCK_STREAM, 0, &ns_errno)))
     {
	fprintf(stderr, "nsock_new: %s\n", nsock_strerror_full_n(ns_errno));
	return 1;
     }
   
   if (c > 2)
     ns->opt |= NSF_USE_FAMILY_HINT;
   if (c > 3)
     ns->opt |= NSF_NO_REVERSE_NAME;
   
   res_ret = nsock_inet_resolve_fwd(ns, v[1], &(ns->inet_tin));
   if (res_ret != NSERR_SUCCESS)
     {
	fprintf(stderr, "resolve failure %d: %s\n", res_ret, nsock_strerror_full(ns));
	return 1;
     }
   
   res_ret = nsock_inet_resolve_rev(ns, &(ns->inet_tin), addr, sizeof(addr));

/*
   addrlen = sizeof(addr);
   switch (ns->inet_tin.ss_family)
     {
      case PF_INET:
	sinp = (struct sockaddr_in *)&ns->inet_tin;
	addr_p = &(sinp->sin_addr);
	port = ntohs(sinp->sin_port);
	break;
	
      case PF_INET6:
	sin6p = (struct sockaddr_in6 *)&ns->inet_tin;
	addr_p = &(sin6p->sin6_addr);
	port = ntohs(sin6p->sin6_port);
	break;
     }
   
   if (inet_ntop(ns->inet_tin.ss_family,
		 addr_p, 
		 addr, addrlen))
 */
   if (res_ret == NSERR_SUCCESS)
     printf("success: %s\n", addr);
   else
     fprintf(stderr, "reverse failure %d: %s\n", res_ret, nsock_strerror_full(ns));
   nsock_free(&ns);
   return 0;
}
