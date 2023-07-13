#ifndef _TREE_SKEL_PRIVATE_H
#define _TREE_SKEL_PRIVATE_H

typedef struct request_t {
    int op_n; //o número da operação
    int op; //a operação a executar. op=0 se for um delete, op=1 se for um put
    char* key; //a chave a remover ou adicionar
    struct data_t *data; // os dados a adicionar em caso de put, ou NULL em caso de delete
    struct request_t *next;
    //adicionar campo(s) necessário(s) para implementar fila do tipo produtor/consumidor
} request_t;

typedef struct {

    int max_proc;
    int *in_progress;
} op_proc;

// Função de controlo de entrada
void comeca(int thread_number);

// Função de controlo de saída
void termina();

#endif