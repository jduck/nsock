/*
 * nsock error reporting routines..
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "nsock_errors.h"


/*
 * fill the error buf it is non-null..
 * 
 * return the passed in integer..
 */
int
nsock_error(nse, nsen, ebuf, ebl, err)
   nsockerror_t *nse;
   u_char *ebuf;
   u_int nsen, ebl;
   int err;
{
   int i = err;
   
   /* no error buf?  return it! */
   if (!ebuf || ebl < 1)
     return err;
   
   /* get the error message and errno string if needed.. */
   if (err < 0)
     i = err * -1;
   if (i < 1 || i > nsen)
     i = 0;
   if (nse[i].has_errno)
     snprintf(ebuf, ebl - 1, "%s: %s",
	      nse[i].str, strerror(errno));
   else
     strncpy(ebuf, nse[i].str, ebl - 1);
   ebuf[ebl - 1] = '\0';
   return err;
}
