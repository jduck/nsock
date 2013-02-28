
void read_eof(void);
void read_timedout(void);
void read_valid(void);
void tcp_connect_bind_fail(void);
void tcp_connect_invalid_dest(void);
void tcp_connect_invalid_dest_port(void);
void tcp_connect_invalid_src(void);
void tcp_connect_invalid_src_port(void);
void tcp_connect_no_route(void);
void tcp_connect_refused(void);
void tcp_connect_timedout(void);
void tcp_connect_valid(void);
void tcp_listen_bad_host(void);
void tcp_listen_bad_port(void);
void tcp_listen_bind_fail(void);

int
main(void)
{
    read_eof();
    read_timedout();
    read_valid();
    tcp_connect_bind_fail();
    tcp_connect_invalid_dest();
    tcp_connect_invalid_dest_port();
    tcp_connect_invalid_src();
    tcp_connect_invalid_src_port();
    tcp_connect_no_route();
    tcp_connect_refused();
    tcp_connect_timedout();
    tcp_connect_valid();
    tcp_listen_bad_host();
    tcp_listen_bad_port();
    tcp_listen_bind_fail();
    return 0;
}
