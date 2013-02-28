CC = gcc
CFLAGS = -Wall -ggdb -I..
LDFLAGS = -L.. -lnsock

include srcobj.mk

all: nsock_test

nsock_test: nsock_test.o $(OBJS)
	$(CC) $(CFLAGS) -o nsock_test nsock_test.o $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o nsock_test

.c.o:
	$(CC) $(CFLAGS) -c $<

# gcc -MM dependencie output
nsock_test.o: nsock_test.c
read_eof.o: read_eof.c nsock_test.h ../nsock_tcp.h ../nsock.h \
  ../nsock_defs.h
read_timedout.o: read_timedout.c nsock_test.h ../nsock_tcp.h ../nsock.h \
  ../nsock_defs.h
read_valid.o: read_valid.c nsock_test.h ../nsock_tcp.h ../nsock.h \
  ../nsock_defs.h
tcp_connect_bind_fail.o: tcp_connect_bind_fail.c nsock_test.h \
  ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_invalid_dest.o: tcp_connect_invalid_dest.c nsock_test.h \
  ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_invalid_dest_port.o: tcp_connect_invalid_dest_port.c \
  nsock_test.h ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_invalid_src.o: tcp_connect_invalid_src.c nsock_test.h \
  ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_invalid_src_port.o: tcp_connect_invalid_src_port.c \
  nsock_test.h ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_no_route.o: tcp_connect_no_route.c nsock_test.h \
  ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_refused.o: tcp_connect_refused.c nsock_test.h ../nsock_tcp.h \
  ../nsock.h ../nsock_defs.h
tcp_connect_timedout.o: tcp_connect_timedout.c nsock_test.h \
  ../nsock_tcp.h ../nsock.h ../nsock_defs.h
tcp_connect_valid.o: tcp_connect_valid.c nsock_test.h ../nsock_tcp.h \
  ../nsock.h ../nsock_defs.h
tcp_listen_bad_host.o: tcp_listen_bad_host.c nsock_test.h ../nsock_tcp.h \
  ../nsock.h ../nsock_defs.h
tcp_listen_bad_port.o: tcp_listen_bad_port.c nsock_test.h ../nsock_tcp.h \
  ../nsock.h ../nsock_defs.h
tcp_listen_bind_fail.o: tcp_listen_bind_fail.c nsock_test.h \
  ../nsock_tcp.h ../nsock.h ../nsock_defs.h
