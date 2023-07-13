#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "entry.h"

struct entry_t *entry_create(char *key, struct data_t *data) {

    if(key == NULL) {
        return NULL;
    }

    struct entry_t *newEntry = malloc(sizeof(struct entry_t));

    newEntry->key = key;
    newEntry->value = data;

    return newEntry;
}

void entry_destroy(struct entry_t *entry) {

    if(entry == NULL) {
        return;
    }

    free(entry->key);
    data_destroy(entry->value);
    free(entry);
}

struct entry_t *entry_dup(struct entry_t *entry) {

    if(entry == NULL || entry->value == NULL) {
        return NULL;
    } 

    struct entry_t *newEntry = malloc(sizeof(struct entry_t));

    newEntry->key = malloc(sizeof(char) * strlen(entry->key) + 1);
    memcpy(newEntry->key, entry->key, sizeof(char) * sizeof(entry->key));
    newEntry->value = data_dup(entry->value);
    
    return newEntry;
}

void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value) {

    free(entry->key);
    free(entry->value);

    entry->key = new_key;
    entry->value = new_value;
}

int entry_compare(struct entry_t *entry1, struct entry_t *entry2) {
    
    if(strcmp(entry1->key, entry2->key) < 0) {
        return -1;
    } else if(strcmp(entry1->key, entry2->key) > 0) {
        return 1;
    } else {
        return 0;
    }
}