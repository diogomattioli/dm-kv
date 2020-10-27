#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

#include <stdint.h>

typedef struct _tree tree;

void *avl_find(tree *_tree, uint32_t id);

void avl_delete(tree *_tree, uint32_t id);

void avl_insert(tree *_tree, uint32_t id, void *data);

void avl_update(tree *_tree, uint32_t id, void *data);

tree *avl_create(void *data);

void avl_destroy(tree *_tree);

#endif // __AVL_TREE_H__