#include <stdio.h>
#include <stdlib.h>
#include <client_stub-private.h>
#include <message-private.h>
#include <network_client.h>

#include <inet-private.h>

int network_connect(struct rtree_t *rtree) {

    if ((rtree->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar socket TCP");
        return -1;
    }

    rtree->socket->sin_family = AF_INET;
    rtree->socket->sin_port = htons(rtree->port);

    if (inet_pton(AF_INET, rtree->host, &rtree->socket->sin_addr) < 1) {
        printf("Erro ao converter IP\n");
        close(rtree->sockfd);
        return -1;
    }

    if (connect(rtree->sockfd,(struct sockaddr *) rtree->socket, sizeof(*rtree->socket)) < 0) {
        perror("Erro ao conectar-se ao servidor");
        close(rtree->sockfd);
        return -1;
    }

    return 0;
}

int network_close(struct rtree_t * rtree) {

    close(rtree->sockfd);
    free(rtree->socket);
    free(rtree->host);
    
    return 0;
}   

MessageT *network_send_receive(struct rtree_t * rtree, MessageT *msg) {
    write_all(rtree->sockfd, msg);
    
    return read_all(rtree->sockfd);
}
