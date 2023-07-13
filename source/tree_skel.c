#include <inet-private.h>
#include <pthread.h>
#include "tree.h"
#include "tree_skel.h"
#include "tree_skel-private.h"

struct tree_t *tree;
op_proc *queue;
request_t *queue_head;

int last_assigned = 1;
int n_threads = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke().
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init(int N)
{
    pthread_t thread[N];
    int thread_param[N];

    queue = malloc(sizeof(op_proc));
    queue->max_proc = 0;
    queue->in_progress = calloc(N, sizeof(int));

    n_threads = N;

    tree = tree_create();
    if (tree == NULL)
    {
        return -1;
    }

    for (int i = 0; i < N; i++)
    {
        thread_param[i] = i + 1;
        if (pthread_create(&thread[i], NULL, &process_request, (void *)&thread_param[i]) != 0)
        {
            printf("\nThread %d não criada.\n", i);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy()
{

    free(queue->in_progress);
    free(queue);

    request_t *tmp; 

    if(queue_head != NULL) {

        while (queue_head->next != NULL) 
        {
            tmp = queue_head;
            queue_head = queue_head->next;
            free(tmp);
        }
        
    }

    tree_destroy(tree);
}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
 */
int invoke(MessageT *msg)
{
    if (msg->opcode == MESSAGE_T__OPCODE__OP_SIZE && msg->c_type == MESSAGE_T__C_TYPE__CT_NONE)
    {
        msg->data_size = tree_size(tree);
        if (msg->data_size < 0)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        else
        {
            msg->opcode += 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            return 0;
        }
    }
    else if (msg->opcode == MESSAGE_T__OPCODE__OP_HEIGHT && msg->c_type == MESSAGE_T__C_TYPE__CT_NONE)
    {
        msg->data_size = tree_height(tree);
        if (msg->data < 0)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        else
        {
            msg->opcode += 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
            return 0;
        }
    }
    else if (msg->opcode == MESSAGE_T__OPCODE__OP_DEL && msg->c_type == MESSAGE_T__C_TYPE__CT_KEY)
    {
        printf("Last assigned -> %d\n", last_assigned);

        request_t *tail;
        request_t *request = malloc(sizeof(request_t));
        request->op_n = last_assigned;
        request->op = 0;
        request->key = msg->keys[0];

        pthread_mutex_lock(&m);

        if (queue_head == NULL) {
            
            queue_head = request;
            request->next = NULL;
        } else {
            tail = queue_head;
            while (tail != NULL)
            {
                tail = queue_head->next;
            }
            tail->next = request;
            request->next = NULL;
        }


        msg->opcode += 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
        msg->op_n = last_assigned;

        last_assigned += 1;

        pthread_cond_signal(&c);
        pthread_mutex_unlock(&m);

        return 0;

    }
    else if (msg->opcode == MESSAGE_T__OPCODE__OP_GET && msg->c_type == MESSAGE_T__C_TYPE__CT_KEY)
    {

        struct data_t *data = tree_get(tree, msg->keys[0]);

        if (data == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        else
        {
            msg->data_size = data->datasize;
            msg->data = (char *) data->data;
            msg->opcode += 1;
            return 0;
        }
    }
    else if (msg->opcode == MESSAGE_T__OPCODE__OP_PUT && msg->c_type == MESSAGE_T__C_TYPE__CT_ENTRY)
    {
        printf("Last assigned -> %d\n", last_assigned);

        request_t *tail;
        request_t *request = malloc(sizeof(request_t));
        request->op_n = last_assigned;
        request->op = 1;
        request->key = msg->keys[0];
        request->data = data_create2(msg->data_size, (void *) msg->data);

        pthread_mutex_lock(&m);

        if (queue_head == NULL) {
            
            queue_head = request;
            request->next = NULL;
        } else {
            tail = queue_head;
            while (tail != NULL)
            {
                tail = queue_head->next;
            }
            tail->next = request;
            request->next = NULL;
        }

        msg->opcode += 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
        msg->op_n = last_assigned;

        last_assigned += 1;

        pthread_cond_signal(&c);
        pthread_mutex_unlock(&m);

        return 0;

    }
    else if (msg->opcode == MESSAGE_T__OPCODE__OP_GETKEYS && msg->c_type == MESSAGE_T__C_TYPE__CT_NONE)
    {
        msg->keys = tree_get_keys(tree);
        if (msg->keys == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        else
        {
            msg->opcode += 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;
            return 0;
        }
    }
    else if (msg->opcode == MESSAGE_T__OPCODE__OP_GETVALUES && msg->c_type == MESSAGE_T__C_TYPE__CT_NONE)
    {
        void **values = tree_get_values(tree);
        if (values == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        else
        {
            msg->opcode += 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_VALUES;
            return 0;
        }
    }
    else if(msg->opcode == MESSAGE_T__OPCODE__OP_VERIFY && msg->c_type == MESSAGE_T__C_TYPE__CT_RESULT) {

        int res = verify(msg->op_n);

        if(res == 0) {
            msg->opcode += 1;
            msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
        } else {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
        }

        msg->op_n = res;
        return res;
    }
    return -1;
}

int verify(int op_n) {

    int len = n_threads;
    int result = 0;

    for(int i = 0; i < len; i++) {

        if(queue->in_progress[i] == op_n) {
            result = -1;
        }
    }

    return result;
}

void *process_request (void *params) {

    int thread_number = *(int *) params;

    pthread_mutex_lock(&m);

    while(queue_head == NULL) {
        pthread_cond_wait(&c, &m);
    }

    request_t *request = queue_head;
    queue_head = request->next;

    queue->in_progress[thread_number-1] = request->op_n;

    if(request->op_n > queue->max_proc) {
        queue->max_proc = request->op_n;
    }

    if(request->op == 0) {
        tree_del(tree, request->key);
    } else {
        tree_put(tree, request->key, request->data);
    }

    queue->in_progress[thread_number] = 0;
    pthread_mutex_unlock(&m);
    
}