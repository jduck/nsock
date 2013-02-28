/*
 * includes things needed for error reporting
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#ifndef __NSOCK_ERRORS_H_
#define __NSOCK_ERRORS_H_

typedef struct __nsock_errors
{
   const u_char *str; 	/* error description */
   u_char has_errno; 	/* errno set? */
} nsockerror_t;

/*	return type	decleration			*/
extern int     		nsock_error(nsockerror_t *, u_int, u_char *, u_int, int);

#endif
