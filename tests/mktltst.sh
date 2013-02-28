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
   if (!(nst = nsock_tcp_init_listen($ON,
				      NSTCP_NONE, eb, sizeof(eb))))
     {
	printf("FAILED   nsock_tcp_listen_init failed\n");
	_exit(1);
     }
   
   /* try to listen */
   sd = nsock_tcp_listen(nst, 1);
   nsock_tcp_free(&nst);
   if (sd < 1)
     {
	if ($EXPR) 	/* XXX: hardcoded return value */
	  {
	     printf("PASSED   nsock_tcp_listen: %s\n", eb);
	     return;
	  }
	/* wrong error? */
	printf("FAILED   nsock_tcp_listen: %s\n", eb);
	_exit(1);
     }
   
   /* listening? */
   close(sd);
   printf("FAILED   nsock_tcp_listen succeeded\n");
   _exit(1);
}
_EOF_

exit 0
