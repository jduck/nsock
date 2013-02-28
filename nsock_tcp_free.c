/*
 * small thing to free anything that is allocated (not to be used with
 * non-allocated structs)
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock_tcp.h"

#include <stdlib.h>

void
nsock_tcp_free(nstp)
   nsocktcp_t **nstp;
{
   nsocktcp_t *nst = *nstp;
   
   if (nst)
     {
	if (nst->from)
	  {
	     free(nst->from);
	     nst->from = NULL;
	  }
	if (nst->to)
	  {
	     free(nst->to);
	     nst->to = NULL;
	  }
	free(nst);
     }
   *nstp = NULL;
}
