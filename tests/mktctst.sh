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
cat << _EOF_
/*
 * code to test libnsock:
 * $DESC
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include "nsock_test.h"
#include "nsock_tcp.h"


void
$FUNC(void)
{
   char eb[256];
   int sd;
   nsocktcp_t *nst;
   
   /* print what we are testing.. */
   printf("%-32s", "$FUNC");
   fflush(stdout);
   
   /* try to initialize the struct */
   if (!(nst = nsock_tcp_init_connect($FROM, $TARG,
				      NSTCP_NONE, eb, sizeof(eb))))
     {
	printf("FAILED   nsock_tcp_connect_init failed\n");
	_exit(1);
     }
   
   /* try to connect */
   sd = nsock_tcp_connect(nst, 0);
   nsock_tcp_free(&nst);
   if (sd < 1)
     {
	if ($EXPR) 	/* XXX: hardcoded return value */
	  {
	     printf("PASSED   nsock_tcp_connect: %s\n", eb);
	     return;
	  }
	/* wrong error? */
	printf("FAILED   nsock_tcp_connect: %s\n", eb);
	_exit(1);
     }
   
   /* connection established? */
   close(sd);
   printf("FAILED   nsock_tcp_connect succeeded\n");
   _exit(1);
}
_EOF_

exit 0
