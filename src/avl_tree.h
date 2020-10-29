#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

#include <stdint.h>

typedef struct avl_tree_t avl_tree_t;

void *avl_find(avl_tree_t *tree, uint32_t id);

void avl_delete(avl_tree_t *tree, uint32_t id);

void avl_insert(avl_tree_t *tree, uint32_t id, void *data);

void avl_update(avl_tree_t *tree, uint32_t id, void *data);

avl_tree_t *avl_create(void *data);

void avl_destroy(avl_tree_t *tree);

#endif // __AVL_TREE_H__