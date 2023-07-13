#include "tree_skel.h"
#include "network_server.h"
#include "message-private.h"
#include "inet-private.h"
#include <poll.h>

int listening_socket;

int network_server_init(short port)
{
    int sockfd;
    struct sockaddr_in server;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Erro ao criar socket");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed\n");
    }

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Erro ao fazer bind");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 0) < 0)
    {
        perror("Erro ao executar listen");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int network_main_loop(int listening_socket)
{
    struct sockaddr_in client;
    socklen_t size_client;
    int nfds = 1;
    int size = 5;
    struct pollfd desc_set[size];
    MessageT *message;

    desc_set[0].fd = listening_socket;
    desc_set[0].events = POLLIN;

    while (poll(desc_set, nfds, 10) >= 0)
    {
        if (desc_set[0].revents && POLLIN)
        {
            desc_set[nfds].fd = accept(listening_socket, (struct sockaddr *)&client, &size_client);
            desc_set[nfds].events = POLLIN;
            nfds++;
        }
        for (int i = 1; i < nfds; i++)
        {
            if (desc_set[i].revents & POLLIN)
            {
                message = network_receive(desc_set[i].fd);
                if (message == NULL)
                {
                    perror("Erro ao receber dados do cliente");
                    close(desc_set[i].fd);
                    desc_set[i].fd = -1;
                }
                else
                {
                    invoke(message);                       /* Executa pedido contido em message */
                    network_send(desc_set[i].fd, message); /* Envia resposta contida em message */
                }
            }
            if (desc_set[i].fd == -1 && POLLHUP)
            {
                close(desc_set[i].fd);
                desc_set[i].fd = -1;
            }
        }
    }

    /*
    printf("Server on\n");

    while ((connsockfd = accept(listening_socket, (struct sockaddr *)&client, &size_client)) != -1)
    {
        MessageT *msgbuf;
        while ((msgbuf = network_receive(connsockfd)) != NULL)
        {
            invoke(msgbuf);
            network_send(connsockfd, msgbuf);
        }
    }
    */
    return 0;
}

MessageT *network_receive(int client_socket)
{
    return read_all(client_socket);
}

int network_send(int client_socket, MessageT *msg)
{
    int m = write_all(client_socket, msg);
    message_t__free_unpacked(msg, NULL);
    return m;
}

int network_server_close()
{
    return close(listening_socket);
}