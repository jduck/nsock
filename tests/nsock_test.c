
void inet_co_bind_failed(void);
void inet_co_init_bad_dest(void);
void inet_co_init_bad_dport(void);
void inet_co_init_bad_sport(void);
void inet_co_init_bad_src(void);
void inet_co_no_route(void);
void inet_co_refused(void);
void inet_co_success(void);
void inet_co_timeout(void);
void inet_listen_bad_host(void);
void inet_listen_bad_port(void);
void inet_listen_bind_failure(void);
void unix_resolve_long(void);
void unix_resolve_path_too_long(void);
void unix_resolve_short(void);
void unix_resolve_sock_too_long(void);

int
main(void)
{
    inet_co_bind_failed();
    inet_co_init_bad_dest();
    inet_co_init_bad_dport();
    inet_co_init_bad_sport();
    inet_co_init_bad_src();
    inet_co_no_route();
    inet_co_refused();
    inet_co_success();
    inet_co_timeout();
    inet_listen_bad_host();
    inet_listen_bad_port();
    inet_listen_bind_failure();
    unix_resolve_long();
    unix_resolve_path_too_long();
    unix_resolve_short();
    unix_resolve_sock_too_long();
    return 0;
}
