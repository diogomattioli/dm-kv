#include <stdlib.h>
#include <string.h>

#include "avl_tree.h"
#include "pointer.h"

typedef struct avl_tree_t avl_tree_t;
typedef struct avl_node_t avl_node_t;

struct avl_tree_t
{
    avl_node_t *root;
};

struct avl_node_t
{
    uint32_t id;
    uint8_t height: 6; // height up to 64

    avl_node_t *left;
    avl_node_t *right;

    void *data;
};

static int height(avl_node_t *node)
{
    if (node->left != NULL && node->right != NULL)
    {
        if (node->left->height > node->right->height)
            return node->left->height + 1;
        else
            return node->right->height + 1;
    }
    else if (node->left != NULL)
        return node->left->height + 1;
    else if (node->right != NULL)
        return node->right->height + 1;

    return 1;
}

static int factor(avl_node_t *node)
{
    if (node->left != NULL && node->right != NULL)
        return node->left->height - node->right->height;
    else if (node->left != NULL)
        return node->left->height;
    else if (node->right != NULL)
        return node->right->height * -1;

    return 1;
}

static avl_node_t *rr_rotate(avl_node_t *node)
{
    avl_node_t *temp = node->right;
    node->right = temp->left;
    temp->left = node;

    node->height = height(node);
    temp->height = height(temp);

    return temp;
}

static avl_node_t *ll_rotate(avl_node_t *node)
{
    avl_node_t *temp = node->left;
    node->left = temp->right;
    temp->right = node;

    node->height = height(node);
    temp->height = height(temp);

    return temp;
}

static avl_node_t *lr_rotate(avl_node_t *node)
{
    avl_node_t *temp = node->left;
    node->left = rr_rotate(temp);
    return ll_rotate(node);
}

static avl_node_t *rl_rotate(avl_node_t *node)
{
    avl_node_t *temp = node->right;
    node->right = ll_rotate(temp);
    return rr_rotate(node);
}

// factor +2   left +1  = LL
// factor +2   left  0  = LL
// factor +2   left -1  = LR
// factor -2  right +1  = RL
// factor -2  right  0  = RR
// factor -2  right -1  = RR
static avl_node_t *balance(avl_node_t *node)
{
    int _factor = factor(node);
    if (_factor > 1) // +2
    {
        _factor = factor(node->left);
        if (_factor < 0) // -1
            node = lr_rotate(node);
        else // +1 0
            node = ll_rotate(node);
    }
    else if (_factor < -1) // -2
    {
        _factor = factor(node->right);
        if (_factor > 0) // +1
            node = rl_rotate(node);
        else // -1 0
            node = rr_rotate(node);
    }

    return node;
}

static avl_node_t *insert(avl_node_t *node, uint32_t id, void *data)
{
    if (node == NULL)
    {
        node = malloc(sizeof(avl_node_t));
        if (node != NULL)
        {
            memset(node, 0, sizeof(avl_node_t));
            node->id = id;
            node->height = 1;
            node->data = data;
        }
        return node;
    }
    else if (node->id > id) // left
        node->left = insert(node->left, id, data);
    else if (node->id < id) // right
        node->right = insert(node->right, id, data);
    else // collision
    {
        // TODO
    }

    node->height = height(node);
    node = balance(node);

    return node;
}

void avl_insert(avl_tree_t *tree, uint32_t id, void *data)
{
    if (data != NULL)
        tree->root = insert(tree->root, id, data);
}

static void swap(avl_node_t *node, avl_node_t *mvn)
{
    avl_node_t temp = *node;

    node->id = mvn->id;
    node->data = mvn->data;
    mvn->data = temp.data;
}

static void node_free(avl_node_t *node)
{
    ptr_free(node->data);
    free(node);
}

static void cascade(avl_node_t *node)
{
    if (node == NULL)
        return;

    cascade(node->left);
    cascade(node->right);

    node_free(node);
}

static avl_node_t *delete(avl_node_t *node, uint32_t id)
{
    if (node == NULL)
        return NULL;
    else if (node->id > id) // left
        node->left = delete(node->left, id);
    else if (node->id < id) // right
        node->right = delete(node->right, id);
    else // found
    {
        if (node->left != NULL)
        {
            avl_node_t *mvn = node->left;
            while (mvn->right != NULL)
                mvn = mvn->right;

            swap(node, mvn);
            node->left = delete(node->left, mvn->id);
        }
        else if (node->right != NULL)
        {
            avl_node_t *mvn = node->right;
            while (mvn->left != NULL)
                mvn = mvn->left;

            swap(node, mvn);
            node->right = delete(node->right, mvn->id);
        }
        else
        {
            node_free(node);
            return NULL;
        }
    }

    node->height = height(node);
    node = balance(node);

    return node;
}

void avl_delete(avl_tree_t *tree, uint32_t id)
{
    tree->root = delete(tree->root, id);
}

static avl_node_t *find(avl_node_t *node, uint32_t id)
{
    while (node != NULL)
    {
        if (node->id > id) // left
            node = node->left;
        else if (node->id < id) // right
            node = node->right;
        else // found
            return node;
    }

    return NULL;
}

void *avl_find(avl_tree_t *tree, uint32_t id)
{
    avl_node_t *node = find(tree->root, id);
    return node != NULL ? node->data : NULL;
}

void avl_update(avl_tree_t *tree, uint32_t id, void *data)
{
    if (data == NULL)
        return;

    avl_node_t *node = find(tree->root, id);
    if (node != NULL)
    {
        ptr_free(node->data);
        node->data = data;
    }
}

avl_tree_t *avl_create(void *data)
{
    avl_tree_t *tree = ptr_malloc(sizeof(avl_tree_t), PTR_TREE, data);
    if (tree == NULL)
        return NULL;

    memset(tree, 0, sizeof(avl_tree_t));

    return tree;
}

void avl_destroy(avl_tree_t *tree)
{
    cascade(tree->root);
    ptr_free(tree);
}