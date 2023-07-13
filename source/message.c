#include "message-private.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "inet-private.h"
#include <errno.h>

MessageT *read_all(int socket)
{

    int size;

    int readSize = read(socket, &size, sizeof(int));

    if (readSize != sizeof(int))
    {
        perror("Read failed:");
        close(socket);
        return NULL;
    }

    uint8_t *buffer = malloc(size);

    int readBuffer = read(socket, buffer, size);

    if (readBuffer != size)
    {
        perror("Read failed:");
        close(socket);
        return NULL;
    }

    MessageT *message;

    message = message_t__unpack(NULL, size, buffer);
    free(buffer);

    return message;
}

int write_all(int sock, MessageT *message)
{
    int size = message_t__get_packed_size(message);

    uint8_t *buffer;
    buffer = malloc(size);

    message_t__pack(message, buffer);

    // Escrever tamanho
    int sizeRes = write(sock, &size, sizeof(size));

    if (sizeRes != sizeof(int))
    {
        perror("Write failed:");
        close(sock);
        return -1;
    }

    // Escrever mensagem
    int res = write(sock, buffer, size);

    if (res != size)
    {
        perror("Write failed:");
        close(sock);
        return -1;
    }

    free(buffer);
    return 0;
}