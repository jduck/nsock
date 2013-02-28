/*
 * fdprintf()
 * 
 * like fprintf() but for file/socket descriptors
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>


ssize_t
nsock_fdprintf(nsock_t *ns, const u_char *fmt, ...)
{
   va_list vl;
   u_char tbuf[8192];
   
   va_start(vl, fmt);
   memset(tbuf, 0, sizeof(tbuf));
   vsnprintf((char *)tbuf, sizeof(tbuf) - 1, (char *)fmt, vl);
   va_end(vl);
   return nsock_write(ns, tbuf, strlen((char *)tbuf));
}
