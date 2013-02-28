/*
 * includes things needed for error reporting
 * 
 * Copyright (C) 2002 Joshua J. Drake <libnsock@qoop.org>
 */
#ifndef __NSOCK_IO_PIPE_H
#define __NSOCK_IO_PIPE_H


#define NSOCK_IOP_BLOCKSZ 	2048
/* the following two defines are currently unused.. */
// #define NSOCK_IOP_READ_TIMEOUT 	10
// #define NSOCK_IOP_WRITE_TIMEOUT 	10


/* a little struct to make things look a bit better */
typedef struct __nsock_io_pipe_buf_stru
{
   u_char *buf;
   size_t len;
} iobuf_t;



/* a nice select loop with an optional initial command */
int nsock_io_pipe(int, int, int, int);


#endif
