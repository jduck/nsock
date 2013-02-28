/*
 * small thing to free anything that is allocated (not to be used with
 * non-allocated structs)
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock_unix.h"

#include <stdlib.h>

void
nsock_unix_free(nsup)
   nsockunix_t **nsup;
{
   nsockunix_t *nsu = *nsup;
   
   if (nsu)
     {
	if (nsu->path)
	  {
	     free(nsu->path);
	     nsu->path = NULL;
	  }
	free(nsu);
     }
   *nsup = NULL;
}
