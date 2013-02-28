#!/bin/sh
#
# a script to create a .c test file
#

# check the args..
if [ -z "$5" ]; then
   echo "usage: `basename $0` <function> <from> <to> <expression> <description>"
   exit 1
fi

# put the args in good var names
FUNC=$1
FROM=$2
TARG=$3
EXPR=$4
shift 4
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
   ns_errno = NSERR_SUCCESS;
   nst = nsock_connect_init(PF_INET, SOCK_STREAM, $FROM, $TARG, NSF_NONE, &ns_errno);
   if (ns_errno == $EXPR)
     {
	printf("PASSED   nsock_connect_init failed: %s\n",
	       nsock_strerror_full_n(ns_errno));
        return;
     }
   else if (!nst)
     {
	printf("FAILED   nsock_connect_init failed: %s\n", 
	       nsock_strerror_full_n(ns_errno));
	_exit(1);
     }
   printf("FAILED   nsock_connect_init succeeded!\n");
   nsock_free(&nst);
   _exit(1);
}
_EOF_

exit 0
