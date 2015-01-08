/*
 * pipe data between 2 end points..
 *
 * Copyright (C) 2002,2004 Joshua J. Drake <libnsock@qoop.org>
 */

#include "nsock.h"
#include "nsock_errors.h"
#include "nsock_io_pipe.h"

#ifdef DEBUG_PIPE_BUFS
# include <stdio.h>
#endif
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


#define NSOCK_IOP_ERRS 		8
nsockerror_t nsock_iop_errors[NSOCK_IOP_ERRS] =
{
   { "Unknown error", 			0 },
   { "Unable to obtain memory", 	1 },
   { "select(2) error", 		1 },
   { "socket exception occurred",      	0 },
   { "read error", 			1 },
   { "EOF received while reading",     	0 },
   { "write error", 			1 },
   { "EOF received while writing",     	0 }
};


/* error definitions for code readability */
#define NSOCK_IOP_NO_MEMORY 		-1
#define NSOCK_IOP_SELECT_FAILED 	-2
#define NSOCK_IOP_DESC_EXCEPTION 	-3
#define NSOCK_IOP_READ_ERROR 		-4
#define NSOCK_IOP_READ_EOF 		-5
#define NSOCK_IOP_WRITE_ERROR 		-6
#define NSOCK_IOP_WRITE_EOF 		-7

#define NSOCK_IOP_SUCCESS 		1


/* some helpers */
static void io_pipe_setfds(fd_set *, fd_set *, fd_set *, 
			   size_t, size_t, int, int, int, int);
static ssize_t io_pipe_buf_append(int, iobuf_t *);
static ssize_t io_pipe_buf_flush(int, iobuf_t *);

#if defined(NSOCK_IOP_READ_TIMEOUT) || defined(NSOCK_IOP_WRITE_TIMEOUT) 
static int io_pipe_set_timeout(int);
static int io_pipe_reset_timeout(void);
static void io_pipe_alarm_handler(int);
static void (*old_handler)(int);
#endif


/*
 * use a select() loop to act just as netcat does...
 */
int
nsock_io_pipe(in1_sd, out1_sd, in2_sd, out2_sd)
   int in1_sd, out1_sd, in2_sd, out2_sd;
{
   fd_set rd, wr, ex;
   iobuf_t local = { NULL, 0 };
   iobuf_t remote = { NULL, 0 };
   int sret, high_desc;
   ssize_t len;

   /* if outputs sockets were not specified, assume they are the same
    * as the input sockets */
   if (out1_sd < 0)
     out1_sd = in1_sd;
   if (out2_sd < 0)
     out2_sd = in2_sd;

   /* get some heap memory for our buffers */
   if (!(local.buf = calloc(1, NSOCK_IOP_BLOCKSZ))
       || !(remote.buf = calloc(1, NSOCK_IOP_BLOCKSZ)))
     {
	if (local.buf)
	  free(local.buf);
	return NSOCK_IOP_NO_MEMORY;
     }

   /* loop until there is a problem ... */
   while (1)
     {
	io_pipe_setfds(&rd, &wr, &ex,
		       remote.len, local.len,
		       in1_sd, out1_sd, 
		       out2_sd, in2_sd);

	/* figure out our highest descriptor */
	high_desc = out2_sd;
	if (in1_sd > high_desc)
	  high_desc = in1_sd;
	if (out1_sd > high_desc)
	  high_desc = out1_sd;
	if (in2_sd > high_desc)
	  high_desc = in2_sd;
	high_desc++;
	
	/* wait forever for something to happen.. */
	sret = select(high_desc, &rd, &wr, &ex, NULL);
	if (sret == -1)
	  return NSOCK_IOP_SELECT_FAILED;
	
	/* exception? */
	if (FD_ISSET(in1_sd, &ex)
	    || FD_ISSET(out1_sd, &ex)
	    || FD_ISSET(in2_sd, &ex)
	    || FD_ISSET(out2_sd, &ex))
	  return NSOCK_IOP_DESC_EXCEPTION;

	/* ok to send stuff to remote? */
	if (FD_ISSET(out1_sd, &wr))
	  {
	     len = io_pipe_buf_flush(out1_sd, &local);
	     if (len < 0)
#ifdef DEBUG_PIPE_BUFS
	       {
		  fprintf(stderr, "local %u remote %u\n", local.len, remote.len);
#endif
		  return len;
#ifdef DEBUG_PIPE_BUFS
	       }
#endif
	     if (!(--sret))
	       continue;
	  }
	
	/* ok to send stuff to local? */
	if (FD_ISSET(out2_sd, &wr))
	  {
	     len = io_pipe_buf_flush(out2_sd, &remote);
	     if (len < 0)
#ifdef DEBUG_PIPE_BUFS
	       {
		  fprintf(stderr, "local %u remote %u\n", local.len, remote.len);
#endif
		  return len;
#ifdef DEBUG_PIPE_BUFS
	       }
#endif
	     if (!(--sret))
	       continue;
	  }
	
	/* stuff coming from remote? */
	if (FD_ISSET(in1_sd, &rd))
	  {
	     len = io_pipe_buf_append(in1_sd, &remote);
	     if (len < 0)
#ifdef DEBUG_PIPE_BUFS
	       {
		  fprintf(stderr, "local %u remote %u\n", local.len, remote.len);
#endif
		  return len;
#ifdef DEBUG_PIPE_BUFS
	       }
#endif
	     if (!(--sret))
	       continue;
	  }
	
	/* stuff coming from local? */
	if (FD_ISSET(in2_sd, &rd))
	  {
	     len = io_pipe_buf_append(in2_sd, &local);
	     if (len < 0)
#ifdef DEBUG_PIPE_BUFS
	       {
		  fprintf(stderr, "local %u remote %u\n", local.len, remote.len);
#endif
		  return len;
#ifdef DEBUG_PIPE_BUFS
	       }
#endif
	     if (!(--sret))
	       continue;
	  }
     }
   return NSOCK_IOP_SUCCESS;
}


/*
 * decide which descriptors to select on
 */
static void
io_pipe_setfds(rd, wr, ex, 
	       rbl, lbl, 
	       in1_sd, out1_sd, 
	       in2_sd, out2_sd)
   fd_set *rd, *wr, *ex;
   size_t rbl, lbl;
   int in1_sd, out1_sd;
   int out2_sd, in2_sd;
{
   /* reset the fd_sets.. */
   FD_ZERO(rd);
   FD_ZERO(wr);
   FD_ZERO(ex);
   
   /* want to read more (if buffer size allows) from remote */
   if (rbl >= 0 && rbl < NSOCK_IOP_BLOCKSZ)
     FD_SET(in1_sd, rd);
   
   /* want to read more (if buffer size allows) from local */
   if (lbl >= 0 && lbl < NSOCK_IOP_BLOCKSZ)
     FD_SET(in2_sd, rd);
   
   /* need to write to remote? */
   if (lbl)
     FD_SET(out1_sd, wr);
   
   /* need to write to local? */
   if (rbl)
     FD_SET(out2_sd, wr);

   /* check for exceptions on all */
   FD_SET(in1_sd, ex);
   FD_SET(out1_sd, ex);
   FD_SET(in2_sd, ex);
   FD_SET(out2_sd, ex);
}



/*
 * append to the end of a buffer..
 */
ssize_t
io_pipe_buf_append(sd, io)
   int sd;
   iobuf_t *io;
{
   ssize_t len;

   /* append to the buffer, possibly fill it */
#ifdef NSOCK_IOP_READ_TIMEOUT
   if (NSOCK_IOP_READ_TIMEOUT > 0
       && !io_pipe_set_timeoutNSOCK_IOP_READ_TIMEOUT))
     return -1;
#endif
   len = read(sd, io->buf + io->len,
	      NSOCK_IOP_BLOCKSZ - io->len);
#ifdef NSOCK_IOP_READ_TIMEOUT
   if (NSOCK_IOP_READ_TIMEOUT > 0)
     (void)io_pipe_reset_timeout();
#endif
   switch (len)
     {
      case -1:
	  {
#ifdef NSOCK_IOP_READ_TIMEOUT
	     if (NSOCK_IOP_READ_TIMEOUT > 0 
		 && errno == EINTR)
	       errno = ETIMEDOUT;
#endif
	     return NSOCK_IOP_READ_ERROR;
	  }
	break;
	
      case 0:
	return NSOCK_IOP_READ_EOF;
	break;
	
      default:
	io->len += len;
	break;
     }
   /* ok.. we success full read the stuf... */
   return len;
}


/*
 * flush the buffer and reset it..
 */
ssize_t
io_pipe_buf_flush(sd, io)
   int sd;
   iobuf_t *io;
{
   ssize_t len;

#ifdef NSOCK_IOP_WRITE_TIMEOUT
   if (NSOCK_IOP_WRITE_TIMEOUT > 0
       && !io_pipe_set_timeoutNSOCK_IOP_WRITE_TIMEOUT)
     return -1;
#endif
   len = write(sd, io->buf, io->len);
#ifdef NSOCK_IOP_WRITE_TIMEOUT
   if (NSOCK_IOP_WRITE_TIMEOUT > 0)
     (void)io_pipe_reset_timeout();
#endif
   switch (len)
     {
      case -1:
	  {
#ifdef NSOCK_IOP_WRITE_TIMEOUT
	     if (NSOCK_IOP_WRITE_TIMEOUT > 0 
		 && errno == EINTR)
	       errno = ETIMEDOUT;
#endif
	     return NSOCK_IOP_WRITE_ERROR;
	  }
	break;
	
      case 0:
	return NSOCK_IOP_WRITE_EOF;
	
      default:
	io->len -= len;
	break;
     }

   /* reset buffer.. */
   if (io->len == 0)
     memset(io->buf, 0, NSOCK_IOP_BLOCKSZ);
   return len;
}


#if defined(NSOCK_IOP_READ_TIMEOUT) || defined(NSOCK_IOP_WRITE_TIMEOUT) 

/*
 * setup the signal handler, etc
 */
static int
io_pipe_set_timeout(timeout)
   int timeout;
{
   struct sigaction nsa, osa;
   
   memset(&nsa, 0, sizeof(nsa));
   nsa.sa_handler = io_pipe_alarm_handler;
   if (sigaction(SIGALRM, &nsa, &osa) == -1)
     {
	fprintf(stderr, "%s: sigaction error: %s\n", __func__, strerror(errno));
	return 0;
     }
   old_handler = osa.sa_handler;
   alarm(timeout);
   return 1;
}


/*
 * simple do-nothing alarm handler
 */
static void
io_pipe_alarm_handler(sig)
   int sig;
{
   /* nothing! */
}


/*
 * simple signal handler reset function
 */
static int
io_pipe_reset_timeout(void)
{
   struct sigaction nsa;
   
   memset(&nsa, 0, sizeof(nsa));
   nsa.sa_handler = old_handler;
   if (sigaction(SIGALRM, &nsa, NULL) == -1)
     {
	fprintf(stderr, "%s: sigaction error: %s\n", __func__, strerror(errno));
	return 0;
     }
   alarm(0);
   return 1;
}

#endif
