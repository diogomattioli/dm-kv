#include <stdlib.h>
#include <string.h>

#include "avl_tree.h"
#include "pointer.h"

typedef struct _tree tree;
typedef struct _node node;

struct _tree
{
    node *root;
};

struct _node
{
    uint32_t id;
    uint8_t height: 6; // height up to 64

    node *left;
    node *right;

    void *data;
};

static int height(node *_node)
{
    if (_node->left != NULL && _node->right != NULL)
    {
        if (_node->left->height > _node->right->height)
            return _node->left->height + 1;
        else
            return _node->right->height + 1;
    }
    else if (_node->left != NULL)
        return _node->left->height + 1;
    else if (_node->right != NULL)
        return _node->right->height + 1;

    return 1;
}

static int factor(node *_node)
{
    if (_node->left != NULL && _node->right != NULL)
        return _node->left->height - _node->right->height;
    else if (_node->left != NULL)
        return _node->left->height;
    else if (_node->right != NULL)
        return _node->right->height * -1;

    return 1;
}

static node *rr_rotate(node *_node)
{
    node *temp = _node->right;
    _node->right = temp->left;
    temp->left = _node;

    _node->height = height(_node);
    temp->height = height(temp);

    return temp;
}

static node *ll_rotate(node *_node)
{
    node *temp = _node->left;
    _node->left = temp->right;
    temp->right = _node;

    _node->height = height(_node);
    temp->height = height(temp);

    return temp;
}

static node *lr_rotate(node *_node)
{
    node *temp = _node->left;
    _node->left = rr_rotate(temp);
    return ll_rotate(_node);
}

static node *rl_rotate(node *_node)
{
    node *temp = _node->right;
    _node->right = ll_rotate(temp);
    return rr_rotate(_node);
}

// factor +2   left +1  = LL
// factor +2   left  0  = LL
// factor +2   left -1  = LR
// factor -2  right +1  = RL
// factor -2  right  0  = RR
// factor -2  right -1  = RR
static node *balance(node *_node)
{
    int _factor = factor(_node);
    if (_factor > 1) // +2
    {
        _factor = factor(_node->left);
        if (_factor < 0) // -1
            _node = lr_rotate(_node);
        else // +1 0
            _node = ll_rotate(_node);
    }
    else if (_factor < -1) // -2
    {
        _factor = factor(_node->right);
        if (_factor > 0) // +1
            _node = rl_rotate(_node);
        else // -1 0
            _node = rr_rotate(_node);
    }

    return _node;
}

static node *insert(node *_node, uint32_t id, void *data)
{
    if (_node == NULL)
    {
        _node = malloc(sizeof(node));
        if (_node != NULL)
        {
            memset(_node, 0, sizeof(node));
            _node->id = id;
            _node->height = 1;
            _node->data = data;
        }
        return _node;
    }
    else if (_node->id > id) // left
        _node->left = insert(_node->left, id, data);
    else if (_node->id < id) // right
        _node->right = insert(_node->right, id, data);
    else // collision
    {
        // TODO
    }

    _node->height = height(_node);
    _node = balance(_node);

    return _node;
}

void avl_insert(tree *_tree, uint32_t id, void *data)
{
    if (data != NULL)
        _tree->root = insert(_tree->root, id, data);
}

static void swap(node *_node, node *mvn)
{
    node temp = *_node;

    _node->id = mvn->id;
    _node->data = mvn->data;
    mvn->data = temp.data;
}

static void node_free(node *_node)
{
    ptr_free(_node->data);
    free(_node);
}

static void cascade(node *_node)
{
    if (_node == NULL)
        return;

    cascade(_node->left);
    cascade(_node->right);

    node_free(_node);
}

static node *delete(node *_node, uint32_t id)
{
    if (_node == NULL)
        return NULL;
    else if (_node->id > id) // left
        _node->left = delete(_node->left, id);
    else if (_node->id < id) // right
        _node->right = delete(_node->right, id);
    else // found
    {
        if (_node->left != NULL)
        {
            node *mvn = _node->left;
            while (mvn->right != NULL)
                mvn = mvn->right;

            swap(_node, mvn);
            _node->left = delete(_node->left, mvn->id);
        }
        else if (_node->right != NULL)
        {
            node *mvn = _node->right;
            while (mvn->left != NULL)
                mvn = mvn->left;

            swap(_node, mvn);
            _node->right = delete(_node->right, mvn->id);
        }
        else
        {
            node_free(_node);
            return NULL;
        }
    }

    _node->height = height(_node);
    _node = balance(_node);

    return _node;
}

void avl_delete(tree *_tree, uint32_t id)
{
    _tree->root = delete(_tree->root, id);
}

static node *find(node *_node, uint32_t id)
{
    while (_node != NULL)
    {
        if (_node->id > id) // left
            _node = _node->left;
        else if (_node->id < id) // right
            _node = _node->right;
        else // found
            return _node;
    }

    return NULL;
}

void *avl_find(tree *_tree, uint32_t id)
{
    node *_node = find(_tree->root, id);
    return _node != NULL ? _node->data : NULL;
}

void avl_update(tree *_tree, uint32_t id, void *data)
{
    if (data == NULL)
        return;

    node *_node = find(_tree->root, id);
    if (_node != NULL)
    {
        ptr_free(_node->data);
        _node->data = data;
    }
}

tree *avl_create(void *data)
{
    tree *_tree = ptr_malloc(sizeof(tree), PTR_TREE, data);
    if (_tree == NULL)
        return NULL;

    memset(_tree, 0, sizeof(tree));

    return _tree;
}

void avl_destroy(tree *_tree)
{
    cascade(_tree->root);
    ptr_free(_tree);
}