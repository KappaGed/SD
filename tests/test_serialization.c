#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/serialization.h"

int main(int argc, char const *argv[])
{

    // char *key1 = "12345";
    // char *key2 = "12346";
    char *keys[] = {"123", "234", "345", 0};
    char **newKeys;
    /*
        printf("\nSize of keys= %ld\n", sizeof(keys));
        printf("Size of array= %ld\n", sizeof(keys) / sizeof(keys[0]));
    */
    char *keyBuff;

    // printf("\nKeyArray to Buffer:\n");

    int bufferSize = keyArray_to_buffer(keys, &keyBuff);
    printf("Size = %i\n", bufferSize);

    /*
    newKeys = buffer_to_keyArray(keyBuff, bufferSize);
        int s = 0;
        while (newKeys[s] != 0)
        {
            printf("test key[%d] = %s\n", s, newKeys[s]);
            s++;
        }
    */
    free(keyBuff);

    return 0;
}
