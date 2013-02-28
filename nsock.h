/*
 * includes things needed for sockets in unix
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#ifndef __NSOCK_H_
# define __NSOCK_H_

# include <nsock/defs.h>

/* generic socket includes */
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# ifdef HAVE_SSL
#  include <openssl/ssl.h>
#  include <openssl/err.h>
#  include <openssl/rand.h>
# endif


/* Are we doing non blocking connects ? */
# if defined(NBLOCK_POSIX) || defined(NBLOCK_BSD) || defined(NBLOCK_SYSV)
#  define NON_BLOCKING_CONNECTS
# endif


/* flags that apply to all sockets */
# define NSF_NONE 		0x00000000 	/* no flags */
# define NSF_UNIX_USE_UMASK    	0x00000001 	/* don't set the mode on
							 * unix sockets */
# define NSF_NON_BLOCK        	0x00000002 	/* non-blocking */
# define NSF_RAND_SRC_PORT 	0x00000004 	/* random from port */
# define NSF_REUSE_ADDR        	0x00000008 	/* reuse bind address */
# define NSF_USE_SSL 		0x00000010 	/* use ssl */
# define NSF_USE_FAMILY_HINT 	0x00000020 	/* use the socket family as a hint for
						 * resolution */
# define NSF_NO_REVERSE_NAME 	0x00000040 	/* dont reverse any address to names,
						 * only to numeric representation */
# define NSF_OOB_INLINE 	0x00000080 	/* put OOB data inband */

/* states */
# define NSS_SOCKET_UNALLOC 	0x00000000
# define NSS_SOCKET_OBTAINED 	0x00000001
# define NSS_SOCKET_BOUND 	0x00000002
# define NSS_SOCKET_LISTENING 	0x00000003
# define NSS_SOCKET_CONNECTING 	0x00000004
# define NSS_SOCKET_CONNECTED 	0x00000005
# ifdef HAVE_SSL
#  define NSS_SOCKET_SSLIZED 	0x00000006
# endif


/* no sockaddr_storage? * /
# ifndef sockaddr_storage
#  define sockaddr_storage 	sockaddr_in
#  define ss_family 		sin_family
# endif
 */


/* regular defines */
# define NSOCK_IOP_BLOCKSZ 	2048


/* unix specific data */
typedef struct __nsock_unix_stru
{
   u_char *path; 		/* path to where the socket is */
   struct sockaddr_un uns; 	/* structure for unix connections */
   mode_t mode; 		/* the socket permissions */
} nsock_unix_t;


/* tcp/udp specific data */
typedef struct __nsock_inet_stru
{
   u_char *from; 		/* bind address */
   struct sockaddr_storage fin;
   u_char *to; 			/* dest address */
   struct sockaddr_storage tin;
} nsock_inet_t;


# ifdef HAVE_SSL
/* ssl stuff */
typedef struct __nsock_ssl_stru
{
   SSL_CTX *context;
   SSL *ssl;
   u_char *cert_file;
   u_char *key_file;
   u_long err;
} nsock_ssl_t;
# endif


/*
 * this is the typedef'd structure that holds all of the information
 * for a specific socket connection.
 */
typedef struct __nsock_stru
{
   int domain; 		/* domain in which socket is */
   int type; 		/* type of socket being dealt with */
   int protocol; 	/* protocol being used on this socket (normally 0) */
   int backlog; 	/* greater than 0 force incoming sockets */
   int sd; 		/* socket descriptor */
   u_long state; 	/* the sockets current state */
   u_long opt; 		/* options for the socket */
   u_int connect_timeout;
   u_int read_timeout;
   u_int write_timeout;
   u_int ns_errno; 	/* the nsock error number */

   /* unix path, udp/tcp bind string */
   union
     {
	nsock_inet_t 	ns_inet;
	nsock_unix_t 	ns_unix;
     } nsock_address_u;
   
# define inet_from 	nsock_address_u.ns_inet.from
# define inet_to 	nsock_address_u.ns_inet.to
# define inet_fin      	nsock_address_u.ns_inet.fin
# define inet_tin      	nsock_address_u.ns_inet.tin
# define unix_path 	nsock_address_u.ns_unix.path
# define unix_sun 	nsock_address_u.ns_unix.uns
# define unix_mode 	nsock_address_u.ns_unix.mode

# ifdef HAVE_SSL
   nsock_ssl_t ns_ssl; 	/* ssl structure */
# endif
} nsock_t;


/* nasty global.. */
extern 	u_char 		nsock_initialized;
extern 	int 		getinfo_errno;


/* generic prototypes.. */
extern 	void 		nsock_init(void);

extern 	void 		nsock_free(nsock_t **);
extern 	nsock_t *      	nsock_new(int, int, int, u_int *);

extern 	int 		nsock_socket(nsock_t *);
extern 	int 		nsock_close(nsock_t *);
extern 	int 		nsock_bind(nsock_t *);
extern 	int 		nsock_listen(nsock_t *);
extern 	int 		nsock_accept(nsock_t *, nsock_t *);
extern 	int 		nsock_connect(nsock_t *);
extern 	ssize_t 	nsock_read(nsock_t *, void *, size_t);
extern 	ssize_t 	nsock_readln(nsock_t *, void *, size_t);
extern 	ssize_t 	nsock_write(nsock_t *, void *, size_t);
extern 	ssize_t        	nsock_fdprintf(nsock_t *, const u_char *, ...);
/* a nice select loop pipe */
extern 	int 		nsock_io_pipe(nsock_t *, int, int, nsock_t *, int, int);
# ifdef NON_BLOCKING_CONNECTS
extern 	int    		nsock_is_connected(nsock_t *);
extern 	int 		nsock_set_blocking(nsock_t *, int);
# endif
extern 	int 		nsock_alarm(nsock_t *, u_int);
# define nsock_read_alarm(x) 		nsock_alarm(ns, x->read_timeout)
# define nsock_write_alarm(x) 		nsock_alarm(ns, x->write_timeout)
# define nsock_connect_alarm(x)        	nsock_alarm(ns, x->connect_timeout)
extern 	nsock_t * 	nsock_connect_init(int, int, u_char *, u_char *, u_long, u_int *);
extern 	int 		nsock_connect_out(nsock_t *);
extern 	nsock_t *      	nsock_listen_init(int, int, u_char *, int, u_long, u_int *);



/* inet prototypes */
extern 	int 		nsock_inet_resolve_fwd(nsock_t *, u_char *, struct sockaddr_storage *);
extern 	int 		nsock_inet_resolve_rev(nsock_t *, struct sockaddr_storage *, u_char *, u_int);
extern 	int 		nsock_inet_bind(nsock_t *);

extern 	u_char * 	nsock_inet_host(u_char *, int);
extern 	u_char * 	nsock_inet_host_str(u_char *, u_char *);
extern 	int 		nsock_inet_host_has_port(u_char *);

/* unix prototypes */
extern 	int 		nsock_unix_resolve(u_char *, nsock_t *);
extern 	int 		nsock_unix_bind(nsock_t *);
extern 	int    		nsock_unix_listen(nsock_t *);


/* ssl prototypes */
# ifdef HAVE_SSL
extern 	int 		nsock_ssl_context_init(nsock_t *);
extern 	int 		nsock_ssl_connect(nsock_t *);
extern 	int 		nsock_ssl_accept(nsock_t *);
# endif

#endif
