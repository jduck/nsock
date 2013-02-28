#ifndef NSOCK_TEST_H
#define NSOCK_TEST_H

#define TCP_HOST_VALID 		"qoop.org:80"
#define TCP_HOST_INVALID 	"bad.:80"
#define TCP_HOST_INVALID_PORT 	"127.0.0.1:bad."
#define TCP_HOST_NO_ROUTE 	"10.0.5.250:80"
#define TCP_HOST_BIND_FAILS 	"qoop.org:3133"
#define TCP_HOST_LOCAL_UNUSED 	"127.0.0.1:6423"
#define TCP_HOST_LOCAL_INUSE 	"127.0.0.1:6000"
#define TCP_HOST_WRAPPED 	"elite.net:22"
#define TCP_HOST_BANNER 	"qoop.org:21"

#endif
