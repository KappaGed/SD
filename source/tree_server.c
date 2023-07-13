#include "inet-private.h"
#include "network_server.h"
#include "tree_skel.h"
#include <signal.h>

int socket_de_escuta;

void exitfc(int signum)
{
    printf("\nSocket fechado de maneira inesperada\n");
    network_server_close(socket_de_escuta);
    tree_skel_destroy();
    exit(0);
}

int main(int argc, char *argv[])
{
    int tsi;
    int socket_de_escuta;
    int result;

    if (argc != 3)
    {
        printf("Número errado de argumentos");
        return -1;
    }
    short port = atoi(argv[1]);
    int N = atoi(argv[2]);

    signal(SIGINT, exitfc);
    signal(SIGPIPE, SIG_IGN);

    if ((socket_de_escuta = network_server_init(port)) == -1)
    {
        return socket_de_escuta;
    }
    else
    {
        if ((tsi = tree_skel_init(N)) == -1)
        {
            return tsi;
        }
        else
        {
            printf("Server running...\n");

            if ((result = network_main_loop(socket_de_escuta)) == -1)
            {
                tree_skel_destroy();
                return result;
            }

            return 0;
        }
    }
}
