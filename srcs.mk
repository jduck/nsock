nsock_errors.o: nsock_errors.c nsock_errors.h
nsock_fdprintf.o: nsock_fdprintf.c nsock.h nsock_defs.h
nsock_io_pipe.o: nsock_io_pipe.c nsock.h nsock_defs.h
nsock_nonb.o: nsock_nonb.c nsock.h nsock_defs.h
nsock_read.o: nsock_read.c nsock_tcp.h nsock.h nsock_defs.h
nsock_readln.o: nsock_readln.c nsock_tcp.h nsock.h nsock_defs.h
nsock_resolve.o: nsock_resolve.c nsock_resolve.h nsock.h nsock_defs.h
nsock_tcp_connect.o: nsock_tcp_connect.c nsock_tcp.h nsock.h nsock_defs.h \
  nsock_resolve.h nsock_errors.h
nsock_tcp_free.o: nsock_tcp_free.c nsock_tcp.h nsock.h nsock_defs.h
nsock_tcp_host.o: nsock_tcp_host.c
nsock_tcp_listen.o: nsock_tcp_listen.c nsock_tcp.h nsock.h nsock_defs.h \
  nsock_resolve.h nsock_errors.h
nsock_unix_connect.o: nsock_unix_connect.c nsock_unix.h nsock.h \
  nsock_defs.h nsock_errors.h
nsock_unix_free.o: nsock_unix_free.c nsock_unix.h nsock.h nsock_defs.h
nsock_unix_listen.o: nsock_unix_listen.c nsock_unix.h nsock.h \
  nsock_defs.h nsock_errors.h
nsock_write.o: nsock_write.c nsock_tcp.h nsock.h nsock_defs.h
