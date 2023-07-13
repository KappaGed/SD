#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include "client_stub.h"

struct rtree_t
{
    char *host;
    int port;
    struct sockaddr_in *socket;
    int sockfd;
};

#endif