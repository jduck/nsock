#!/bin/sh
#
# a script to create a .c test file
#

# check the args..
if [ -z "$4" ]; then
   echo "usage: `basename $0` <function> <listen on> <expression> <description>"
   exit 1
fi

# put the args in good var names
FUNC=$1
ON=$2
EXPR=$3
shift 3
DESC=$*

# output the new .c file to stdout
cat > $FUNC.c << _EOF_
/*
 * code to test libnsock:
 * $DESC
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <nsock/nsock.h>
#include <nsock/errors.h>

#include "nsock_test.h"


void
$FUNC(void)
{
   nsock_t *nst;
   u_int ns_errno;
   
   /* print what we are testing.. */
   printf("%-32s", "$FUNC");
   fflush(stdout);
   
   /* try to initialize the struct */
   nst = nsock_listen_init(PF_INET, SOCK_STREAM, $ON, 1, NSF_NONE, &ns_errno);
   if (ns_errno == $EXPR)
     {
	printf("PASSED   nsock_listen_init: %s\n",
	       nsock_strerror_full_n(ns_errno));
	return;
     }
   else if (!nst)
     {
	printf("FAILED   nsock_listen_init: %s\n", 
	       nsock_strerror_full_n(ns_errno));
	_exit(1);
     }
   
   /* listening? */
   printf("FAILED   nsock_listen_init succeeded\n");
   nsock_free(&nst);
   _exit(1);
}
_EOF_

exit 0
