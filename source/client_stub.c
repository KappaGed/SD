#include "data.h"
#include "entry.h"
#include "client_stub.h"
#include "client_stub-private.h"
#include "../include/sdmessage.pb-c.h"
#include "network_client.h"
#include "inet-private.h"

struct rtree_t *rtree_connect(const char *address_port)
{
    if (address_port != NULL)
    {
        char *adress_copy = malloc(strlen(address_port) * sizeof(char));
        strcpy(adress_copy, address_port);

        char *host = strtok(adress_copy, ":");
        char *port = strtok(NULL, ":");

        if (host != NULL && port != NULL) {

            struct rtree_t *rtree = malloc(sizeof(struct rtree_t));
            rtree->socket = malloc(sizeof(struct sockaddr_in));
            rtree->host = host;
            rtree->port = atoi(port);

            if (network_connect(rtree) == -1) {

                free(rtree);
                return NULL;
            }
            return rtree;
        }
    }
    return NULL;
}

int rtree_disconnect(struct rtree_t *rtree)
{
    if (rtree != NULL)
    {
        network_close(rtree);
        free(rtree);
        return 0;
    }
    return -1;
}

int rtree_put(struct rtree_t *rtree, struct entry_t *entry) {

    if (rtree == NULL || entry == NULL)
    {
        return -1;
    }

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);
    
    message->c_type = MESSAGE_T__C_TYPE__CT_ENTRY;
    message->opcode = MESSAGE_T__OPCODE__OP_PUT;
    message->data_size = entry->value->datasize;
    message->data = entry->value->data;
    message->n_keys = 1;
    message->keys = &entry->key;

    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_PUT+1) {
        return -1;
    }

    printf("Pedido %d adicionado com sucesso\n", recMessage->op_n);

    message_t__free_unpacked(recMessage, NULL);

    return 0;
}

struct data_t *rtree_get(struct rtree_t *rtree, char *key) {

    if (rtree == NULL || key == NULL)
    {
        return NULL;
    } 

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_GET;
    message->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    message->keys = &key;
    message->n_keys = 1;

    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_GET+1) {
        printf("recMessage == NULL\n");
        return NULL;
    }

    struct data_t *data = data_create2(recMessage->data_size, (void *) recMessage->data);

    printf("%s\n", (char*) data->data);

    message_t__free_unpacked(recMessage, NULL);

    return data;
}

int rtree_del(struct rtree_t *rtree, char *key) {

    if (rtree == NULL || key == NULL)
    {
        return -1;
    } 

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_DEL;
    message->c_type = MESSAGE_T__C_TYPE__CT_KEY;
    message->keys = &key;
    message->n_keys = 1;

    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_DEL+1) {

        return -1;
    }

    printf("Pedido %d adicionado com sucesso\n", recMessage->op_n);

    message_t__free_unpacked(recMessage, NULL);

    return 0;
}

int rtree_size(struct rtree_t *rtree) {

    if (rtree == NULL) {

        return -1;
    }

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_SIZE;
    message->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    
    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_SIZE+1) {
        return -1;
    }

    message_t__free_unpacked(recMessage, NULL);

    return recMessage->data_size;
}

int rtree_height(struct rtree_t *rtree) {

    if(rtree == NULL) {
        return -1;
    }

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_HEIGHT;
    message->c_type = MESSAGE_T__C_TYPE__CT_NONE;

    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_HEIGHT+1) {

        return -1;
    }

    return recMessage->data_size;
}

char **rtree_get_keys(struct rtree_t *rtree) {

    if (rtree == NULL) {
        return NULL;
    }

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_GETKEYS;
    message->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
    
    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_GETKEYS+1) {

        return NULL;
    }

    char **keys = malloc(recMessage->n_keys * sizeof(char*));

    for(int i = 0; i < recMessage->n_keys; i++) {

        strcpy(keys[i], recMessage->keys[i]);
    }

    message_t__free_unpacked(recMessage, NULL);

    return keys;
}

void **rtree_get_values(struct rtree_t *rtree) {

    //ERRADO
    //TODO

    if (rtree == NULL) {
        return NULL;
    }

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_GETVALUES;
    message->c_type = MESSAGE_T__C_TYPE__CT_VALUES;
    
    MessageT *recMessage = network_send_receive(rtree, message);

    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_GETKEYS+1) {

        return NULL;
    }

    void **values = malloc(recMessage->n_keys * sizeof(void*));

    for(int i = 0; i < recMessage->n_keys; i++) {

        strcpy(values[i], recMessage->keys[i]);
    }

    message_t__free_unpacked(recMessage, NULL);

    return values;
}

int rtree_verify(struct rtree_t *rtree, int op_n) {
    
    if (rtree == NULL || op_n < 0) {
        return -1;
    }

    MessageT *message = malloc(sizeof(MessageT));

    message_t__init(message);

    message->opcode = MESSAGE_T__OPCODE__OP_VERIFY;
    message->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
    message->op_n = op_n;

    MessageT *recMessage = network_send_receive(rtree, message);
    
    if(recMessage == NULL || recMessage->opcode != MESSAGE_T__OPCODE__OP_VERIFY+1) {

        printf("Operação %d ainda não concluida\n", message->op_n);

        return -1;
    } else {

        printf("Operação %d concluída\n", message->op_n);
    }
    
    return recMessage->op_n;
}

