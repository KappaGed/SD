#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "tree.h"
#include "entry.h"

struct tree_t {
	
	struct entry_t *entry;

	struct tree_t *left;
	struct tree_t *right;
};


#endif
