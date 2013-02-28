/*
 * fdprintf()
 * 
 * like fprintf() but for file/socket descriptors
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"

#ifdef FDPRINTF_DEBUG
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>


ssize_t
nsock_fdprintf(int sd, u_int timeout, const u_char *fmt, ...)
{
   va_list vl;
   u_char tbuf[8192];
   
   if (sd < 0)
     return -1;
   va_start(vl, fmt);
   memset(tbuf, 0, sizeof(tbuf));
   vsnprintf(tbuf, sizeof(tbuf)-1, fmt, vl);
   va_end(vl);
#ifdef FDPRINTF_DEBUG
   fprintf(stderr, "to sd #%d: %s", sd, tbuf);
#endif
   return nsock_write(sd, tbuf, strlen(tbuf), timeout);
}
