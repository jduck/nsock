/*
 * code to test all parts of libnsock functionality..
 * 
 * stuff currently tested:
 *   - tcp listen
 *   - tcp connect
 *   - unix listen
 *   - unix connect
 * 
 * to be tested:
 *   - resolver
 *   - force errors
 *   - initialization routines
 *   - udp listen/connect
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include <errno.h>

#include "nsock.h"
#include "nsock_tcp.h"
#include "nsock_unix.h"
#include "nsock_resolve.h"


#define TCP_HOST_LOCAL_UNUSED 	"127.0.0.1:6243"


/* tcp test stuff */
void test_tcp(void);
void test_tcp_listen(void);
void test_tcp_connect(void);

/* unix test stuff */
void test_unix(void);
void test_unix_listen(void);
void test_unix_connect(void);


/*
 * dispatch the tests!
 */
int
tcp_listen_and_connect(c, v)
   int c;
   char *v[];
{
   test_tcp();
   /* test_unix(); */
   return 0;
}


/*
 * test tcp connection stuff...
 * 
 * here we fork off a child to listen for our connection..
 */
void
test_tcp(void)
{
   pid_t cpid;
   int status;
   
   switch ((cpid = fork()))
     {
      case -1:
	perror("fork failed during tcp test");
	exit(1);
	/* not reached */
      case 0:
	/* child */
	sleep(1);
	test_tcp_connect();
	exit(0);
	/* not reached */
      default:
	break;
     }
   /* listen for the child's connection */
   test_tcp_listen();
   /* clean up the child mess */
   if (waitpid(cpid, &status, 0) == -1)
     perror("waitpid failed during tcp test");
}


/*
 * test the tcp connection stuff
 */
void
test_tcp_connect(void)
{
   char eb[256], msg[] = "hello listener\n";
   int sd;
   nsocktcp_t *nst;
   
   printf("test_tcp_connect: connecting to " TCP_HOST_LOCAL_UNUSED "\n");
   
   if (!(nst = nsock_tcp_init_connect(NULL, TCP_HOST_LOCAL_UNUSED, NSTCP_NONE, eb, sizeof(eb))))
     {
	fprintf(stderr, "nsock_tcp_init_connect failed\n");
	return;
     }
   sd = nsock_tcp_connect(nst, 0);
   nsock_tcp_free(&nst);
   if (sd < 1)
     {
	fprintf(stderr, "nsock_tcp_connect: %s\n", eb);
	return;
     }
   if (write(sd, msg, strlen(msg)) != strlen(msg))
     {
	perror("short write during tcp connect test");
	return;
     }
   close(sd);
   printf("test_tcp_connect successfully wrote the message.\n");
   return;
}


/*
 * test tcp listening stuff
 */
void
test_tcp_listen(void)
{
   char eb[256], msg[64];
   struct sockaddr_in sin;
   int sd, asd;
   int sl = sizeof(sin);
   nsocktcp_t *nst;
   
   printf("test_tcp_listen: listening on " TCP_HOST_LOCAL_UNUSED "\n");
   
   if (!(nst = nsock_tcp_init_listen(TCP_HOST_LOCAL_UNUSED, 
				     NSTCP_NONE, eb, sizeof(eb))))
     {
	fprintf(stderr, "nsock_tcp_init_listen failed\n");
	return;
     }
   sd = nsock_tcp_listen(nst, 1);
   nsock_tcp_free(&nst);
   if (sd < 1)
     {
	fprintf(stderr, "nsock_tcp_listen: %s\n", eb);
	return;
     }
   
   /* accept the connection */
   asd = accept(sd, (struct sockaddr *)&sin, &sl);
   if (asd == -1)
     {
	perror("accept failed during tcp listen test");
	close(sd);
	return;
     }
   printf("accepted connection from: %s:%u\n", 
	  inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
   memset(msg, 0, sizeof(msg));
   if (read(asd, msg, sizeof(msg)-1) < 1)
     {
	perror("short read during tcp listen test");
	return;
     }
   close(asd);
   close(sd);
   printf("test_tcp_listen successfully received: %s\n", msg);
   return;
}
