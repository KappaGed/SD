#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tree.h"
#include "../include/tree-private.h"

struct tree_t *tree_create() {
    
    struct tree_t *tree = malloc(sizeof(struct tree_t));

    tree->entry = NULL;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

void tree_destroy(struct tree_t *tree) {

    free(tree->entry);
    free(tree->left);
    free(tree->right);
    free(tree);
}

int tree_put(struct tree_t *tree, char *key, struct data_t *value) {

    if(tree == NULL || key == NULL || value == NULL) {
        return -1;
    }

    struct data_t *data; 
    data = tree_get(tree, key);

    char *keycpy = malloc(sizeof(char*));
    strcpy(keycpy, key);

    struct data_t *datacpy = malloc(sizeof(struct data_t));
    datacpy->data = malloc(value->datasize * sizeof(void*));

    memcpy(datacpy, value, sizeof(struct data_t));

    struct entry_t *newEntry = entry_create(keycpy, datacpy);
    struct entry_t *oldEntry = tree->entry;

    if(data != NULL) {
        data_replace(data, value->datasize, value->data);
        return 0;
    }

    if(tree->entry == NULL) {
        tree->entry = newEntry;
    } 
    else  {

        if(entry_compare(newEntry, oldEntry) == -1) {

            if(tree->left == NULL) {
                tree->left = tree_create();
            } 
            data_destroy(datacpy);
            return tree_put(tree->left, key, value);
        } else if(entry_compare(newEntry, oldEntry) == 1) {

            if(tree->right == NULL) {
                tree->right = tree_create();
            }
            return tree_put(tree->right, key, value);
        } else {
            entry_replace(oldEntry, key, value);
        }
    }

    return 0;
}

struct data_t *tree_get(struct tree_t *tree, char *key) {
    
    if(tree == NULL || tree->entry == NULL) {
        return NULL;
    }

    if (strcmp(key, tree->entry->key) < 0)
    {
        return tree_get(tree->left, key);
    } else if(strcmp(key, tree->entry->key) > 0) {

        return tree_get(tree->right, key);
    } else {

        return data_dup(tree->entry->value);
    }
}

int tree_del(struct tree_t *tree, char *key) {

    struct data_t *data = tree_get(tree, key);

    if(data == NULL) {
        return -1;
    } 

    if(tree->left != NULL) {
        return tree_del(tree->left, key);
    }
    if(tree->right != NULL) {
        return tree_del(tree->right, key);
    }

    if(tree->entry != NULL) {
        entry_destroy(tree->entry);
        return 0;
    } else {
        free(tree);
        return 0;
    }

    return 0;
}

int tree_size(struct tree_t *tree) {

    if(tree == NULL || tree->entry == NULL) {
        return 0;
    } else {
        return(tree_size(tree->left) + 1 + tree_size(tree->right));
    }

}

int tree_height(struct tree_t *tree) {

        if (tree == NULL) {
        return 0;
    } else {
        int lSize = tree_size(tree->left);
        int rSize = tree_size(tree->right);

        if(lSize > rSize) {
            return (lSize + 1);
        } else {
            return (rSize + 1);
        }
    }
}

char **tree_get_keys(struct tree_t *tree) {
    
    int nkeys = tree_size(tree);

    char** keys = malloc(nkeys * sizeof(char*));

    int index = 0;

    if (tree->entry != NULL)
    {
        keys[index] = malloc(sizeof(char*));
        memcpy(keys[index], tree->entry->key, sizeof(strlen(tree->entry->key) * sizeof(char) + 1));

        if (tree->left != NULL)
        {
            return tree_get_keys(tree->left);
        } 

        if (tree->right != NULL)
        {
            return tree_get_keys(tree->right);
        }
    }
    
    return keys;
}

void **tree_get_values(struct tree_t *tree) {
    return NULL;
}

void tree_free_keys(char **keys) {

    int counter = 0;

    while(keys[counter] != NULL) {
        free(keys[counter]);
        counter += 1;
    }

    free(keys);
    
}

void tree_free_values(void **values) {

    int counter = 0;

    while(values[counter] != NULL) {
        free(values[counter]);
        counter += 1;
    }

    free(values);
}