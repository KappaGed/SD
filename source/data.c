#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

struct data_t *data_create(int size) {

    if(size <= 0) {
        return NULL;
    }

    struct data_t *newData = malloc(sizeof(struct data_t));

    newData->data = malloc(size * sizeof(void*));
    newData->datasize = size;

    return newData;
}

struct data_t *data_create2(int size, void *data) {
    
    if(size <= 0 || data == NULL) {
        return NULL;
    }

    struct data_t *newData = malloc(sizeof(struct data_t));

    newData->data = data;
    newData->datasize = size;

    return newData;
}

void data_destroy(struct data_t *data) {

    free(data);
}

struct data_t *data_dup(struct data_t *data) {

    if(data == NULL || data->data == NULL || data->datasize < 0) {
        return NULL;
    }

    struct data_t *newData = malloc(sizeof(struct data_t));

    newData->data = malloc(data->datasize * sizeof(void*));
    newData->datasize = data->datasize;

    memcpy(newData->data, data->data, newData->datasize * sizeof(void*));

    return newData;
}

void data_replace(struct data_t *data, int new_size, void *new_data) {

    if(data == NULL || new_size < 0 || new_data == NULL) {
        return ;
    }

    data->datasize = new_size;
    free(data->data);
    data->data = new_data;
}