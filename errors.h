/* do not modified, automatically generated. modify the .in */

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


/* types of errno that an error can have */
#define NSERRNO_NSOCK 				0
#define NSERRNO_LIBC 				1
#define NSERRNO_RESOLV 				2
#define NSERRNO_SSL 				3


/* errors that should not be used ;) */
#define NSERR_SUCCESS 				0
#define NSERR_UNKNOWN_ERROR    			1

/* generic errors */
#define NSERR_NO_SUPPORT    			2
#define NSERR_NO_STORAGE       			3
#define NSERR_NO_DATA       			4
#define NSERR_NO_SOCKET              		5
#define NSERR_INVALID_DOMAIN         		6
#define NSERR_INVALID_TYPE             		7
#define NSERR_INVALID_STATE         		8
#define NSERR_OUT_OF_MEMORY         		9

/* errors for nsock_read() */
#define NSERR_READ_ERROR 			10
#define NSERR_READ_EOF 				11

/* errors for nsock_write() */
#define NSERR_WRITE_ERROR 			12
#define NSERR_WRITE_EOF        			13

/* errors for nsock_alarm() */
#define NSERR_SIGACTION_FAILED 			14

/* errors for nsock_socket() */
#define NSERR_SOCKET_ALREADY        		15
#define NSERR_SOCKET_FAILED 			16
#define NSERR_SOCKET_SOCKOPT_FAILED    		17

/* errors for nsock_close() */
#define NSERR_CLOSE_FAILED 			18

/* errors for nsock_bind() */
#define NSERR_BIND_UNSUP_DOMAIN        		19
#define NSERR_BIND_FAILED 			20

/* errors for nsock_unix_bind() */
#define NSERR_UNIX_BIND_GETCWD_FAILED 		21
#define NSERR_UNIX_BIND_CHDIR_FAILED 		22
#define NSERR_UNIX_BIND_CHMOD_FAILED 		23

/* errors for nsock_inet_bind() */
#define NSERR_INET_BIND_NO_ADDR 		24
#define NSERR_INET_BIND_BAD_PORT 		25
#define NSERR_INET_BIND_BAD_HOST 		26

/* errors for nsock_listen() */
#define NSERR_LISTEN_FAILED       		27

/* errors for nsock_accept() */
#define NSERR_ACCEPT_FAILED       		28
#define NSERR_ACCEPT_NONE_AVAIL       		29

/* errors for non-blocking stuff */
#define NSERR_NONB_FAILED 		 	30

/* errors for nsock_*_connect() */
#define NSERR_CONNECT_FAILED 			31
#define NSERR_CONNECT_IN_PROGRESS 		32

/* errors for nsock_inet_connect_out() */
#define NSERR_INET_CO_SRC_BAD_PORT	       	33
#define NSERR_INET_CO_SRC_BAD_HOST	       	34
#define NSERR_INET_CO_SRC_BAD_DOMAIN	       	35
#define NSERR_INET_CO_SRC_UNKNOWN	       	36
#define NSERR_INET_CO_DST_BAD_PORT	       	37
#define NSERR_INET_CO_DST_BAD_HOST	       	38
#define NSERR_INET_CO_DST_BAD_DOMAIN	       	39
#define NSERR_INET_CO_DST_UNKNOWN	       	40

/* errors for nsock_inet_listen() */
#define NSERR_INET_LISTEN_BAD_PORT 		41
#define NSERR_INET_LISTEN_BAD_HOST 		42
#define NSERR_INET_LISTEN_UNKNOWN 		43

/* errors for nsock_inet_resolve_*() */
#define NSERR_INET_RESOLVE_NOT_FOUND	       	44
#define NSERR_INET_RESOLVE_BAD_PORT 	       	45
#define NSERR_INET_RESOLVE_BAD_HOST 	       	46

/* errors for nsock_unix_resolve() */
#define NSERR_UNIX_PATH_TOO_LONG 		47
#define NSERR_UNIX_SOCK_TOO_LONG 		48
#define NSERR_UNIX_BAD_PATH 			49

/* errors for nsock_io_pipe() */
#define NSERR_IOP_SELECT_FAILED 		50
#define NSERR_IOP_DESC_EXCEPTION 		51

/* errors for nsock_ssl_context_init */
#define NSERR_SSL_NEW_CONTEXT_FAILED 		52
#define NSERR_SSL_INVALID_CERT 			53
#define NSERR_SSL_INVALID_KEY 			54
#define NSERR_SSL_KEY_CERT_MISMATCH 		55
#define NSERR_SSL_SET_CIPHERS_FAILED 		56

/* errors for ssl library errors */
#define NSERR_SSL_NEW_FAILED 			57
#define NSERR_SSL_SYSCALL 			58
#define NSERR_SSL_SSL 				59
#define NSERR_SSL_UNKNOWN 			60

#define NSERR_COUNT 				61


/* error strings, and has_errno values */
extern 	nsockerror_t 	nsock_errors[NSERR_COUNT];



/*	return type	decleration			*/
extern const u_char *  	nsock_strerror_n(u_int);
extern const u_char *  	nsock_strerror(nsock_t *);

extern u_char *		nsock_strerror_full_n(u_int);
extern u_char *		nsock_strerror_full(nsock_t *);


/* used internally */
extern int     		nsock_error(nsock_t *, u_int);
extern int     		nsock_generic_check(nsock_t *, int, u_long);

#endif
