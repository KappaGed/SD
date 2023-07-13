#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data.h"
#include "../include/entry.h"
#include "../include/serialization.h"

int aux_key_to_buffer(char *key, char **keys_buf)
{
    int key_size = sizeof(key);

    if (keys_buf == NULL)
    {
        return -1;
    }
    //  printf("Key dentro = %s\n", key);
    memcpy(*keys_buf, &key_size, sizeof(int));
    memcpy(*keys_buf + sizeof(int), key, key_size);

    return sizeof(int) + key_size;
}

int keyArray_to_buffer(char **keys, char **keys_buf)
{
    int keys_size = 0;
    int buf_size = 0;
    int tam = 0;

    int s = 0;
    while (keys[s] != 0)
    {
        //     printf("key[%d] %s\n", s, keys[s]);
        keys_size++;
        s++;
    }
    buf_size = keys_size * (sizeof(int) + sizeof(char *));
    //   printf("buf Size = %d, bah = %d \n", buf_size, keys_size);

    *keys_buf = malloc(buf_size);

    if (keys == NULL || keys_buf == NULL)
    {
        return -1;
    }

    for (int i = 0; i < keys_size; i++)
    {
        //       printf("Key for = %s\n", keys[i]);
        tam += aux_key_to_buffer(keys[i], keys_buf);
        buf_size = tam;
    }

    return buf_size;
}

char *aux_buffer_to_key(char *keys_buf, int keys_buf_size)
{
    if (keys_buf == NULL || keys_buf_size <= 0)
        return NULL;

    int tamanho;
    memcpy(&tamanho, keys_buf, sizeof(int));
    char key[tamanho];
    char *pkey = key;
    memcpy(key, keys_buf + sizeof(int), tamanho);
    printf("Key aux = %s\n", key);

    return pkey;
}

/*
char **buffer_to_keyArray(char *keys_buf, int keys_buf_size)
{
    int arraySize = keys_buf_size / (sizeof(int) + sizeof(char *));
    printf("ArraySize = %d\n", arraySize);
    char *keys[arraySize + 1];
    char **pkeys = keys;

    if (keys_buf == NULL || keys_buf_size < 0)
    {
        return NULL;
    }

    for (int i = 0; i <= arraySize; i++)
    {
        //  keys[i] = "prof";
        // keys[i] = aux_buffer_to_key(keys_buf, keys_buf_size);
        strcpy(keys[i], aux_buffer_to_key(keys_buf, keys_buf_size));
        if (i == arraySize)
        {
            keys[i] = 0;
        }
        printf("Key[%d] = %s\n", i, keys[i]);

        // keys[i] = "";
    }

    return pkeys;
}
*/