#include "inet-private.h"

#include "client_stub.h"
#include "client_stub-private.h"
#include <signal.h>

int processInput(char *input, struct rtree_t *rtree)
{
    signal(SIGPIPE, SIG_IGN);

    char *command = strtok(input, " ");

    char key[100];
    char data[1000];
    int op_n;

    if (strcmp(command, "size") == 0)
    {
        // printf("entrou size\n");

        int size = rtree_size(rtree);

        printf("A arvore tem %i elementos\n", size);
    }

    else if (strcmp(command, "height") == 0)
    {

        int height = rtree_height(rtree);

        printf("A arvore tem %i de altura\n", height);
    }
    else if (strcmp(command, "del") == 0)
    {

        scanf("%s", key);

        if (key == NULL)
        {

            printf("Insira uma key valida apos o comando\n");
        }
        else
        {

            if (rtree_del(rtree, key) == -1)
            {
                printf("Erro ao remover a entrada\n");
            }
            else
            {
                printf("Entrada removida com sucesso\n");
            }
        }
    }
    else if (strcmp(command, "get") == 0)
    {
        scanf("%s", key);

        if (key == NULL)
        {

            printf("Insira uma key valida apos o comando\n");
        }
        else
        {

            struct data_t *data = rtree_get(rtree, key);

            if (data == NULL)
            {
                printf("Erro ao adquirir os dados\n");
            }
            else
            {

                printf("-> %s\n", (char *) data->data);
                free(data);
            }
        }
    }
    else if (strcmp(command, "put") == 0)
    {

        scanf("%s", key);
        scanf("%s", data);

        if (key == NULL)
        {

            printf("Insira uma key valida apos o comando\n");
        }
        else if (data == NULL)
        {

            printf("Dados inseridos invalidos\n");
        }
        else
        {

            struct entry_t *entry = entry_create(key, data_create2(strlen(data), (void *)data));

            if (rtree_put(rtree, entry) == -1)
            {

                printf("Erro ao inserir entrada\n");
            }
        }
    }
    else if (strcmp(command, "getkeys") == 0)
    {

        char **keys = rtree_get_keys(rtree);

        int counter = 0;

        char *key = keys[counter];

        printf("Chaves:\n");

        while (key != NULL)
        {
            key = keys[counter];
            printf("-> %s\n", key);
            counter += 1;
        }
    }
    else if (strcmp(command, "getvalues") == 0)
    {

        char **values = rtree_get_keys(rtree);

        int counter = 0;

        char *value = values[counter];

        printf("Valores:\n");

        while (value != NULL)
        {
            value = values[counter];
            printf("-> %s\n", value);
            counter += 1;
        }
    } 
    else if (strcmp(command, "verify") == 0) {

        scanf("%d", &op_n);

        if(rtree_verify(rtree, op_n) == -1) {
            printf("Pedido na fila\n");
        } else {
            printf("Pedido concluido\n");
        }
    }
    else if (strcmp(command, "quit") == 0)
    {
        return 1;
    }
    else
    {
        printf("Comando introduzido invalido\n");
    }

    return 0;
}

int main(int argc, char **argv)
{
    struct rtree_t *rtree;

    printf("%i\n", argc);
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);

    if (argc != 2)
    {
        printf("E necessarios 1 argumento de entrada.\n");
        return -1;
    }

    rtree = rtree_connect(argv[1]);

    if (rtree == NULL)
    {
        return -1;
    }

    int ciclo = 1;

    char input[20];

    while (ciclo == 1)
    {
        printf("Escolha uma operação:\n size\n height\n del <key>\n get <key>\n put <key> <data>\n getkeys\n getvalues\n verify <op_n>\n quit\n -> ");
        scanf("%s", input);

        if (processInput(input, rtree) == 1)
        {
            // ciclo = 0;
            break;
        }
    }

    rtree_disconnect(rtree);
    return 0;
}