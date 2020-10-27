#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "pointer.h"
#include "avl_tree.h"
#include "queue.h"
#include "atomic.h"

#include "query.h"

#define HASH_MULTIPLIER 37

typedef struct _query query;
typedef struct _key key;

struct _query
{
    queue *keys;
    tree *_tree;
    uint32_t id;
    int fd;
    void *data;

    void (*action)(query *_query, void *data);

    bool blocking;
};

struct _key
{
    uint32_t id;
    char *name;
};

static tree *master = NULL;

void query_init()
{
    if (master != NULL)
        return;

    atomic *_atomic = atomic_create();
    master = avl_create(_atomic);
}

static uint32_t hash_multiplication(const char *s)
{
    uint32_t h = 0;
    uint8_t *c = (uint8_t *) s;

    while (*c != '\0')
        h = h * HASH_MULTIPLIER + *c++;

    return h;
}

static void parse(char *query, queue **_queue, char **data)
{
    char *query_keys = strtok(query, ":");
    char *query_data = strtok(NULL, ":");

    char *token = strtok(query_keys, "/");
    while ((token = strtok(NULL, "/")) != NULL)
    {
        key *_key = malloc(sizeof(key));
        _key->id = hash_multiplication(token);
        _key->name = token;
        queue_push(*_queue, _key);
    }

    if (query_data != NULL)
    {
        size_t len = strlen(query_data) + 1;
        *data = ptr_malloc(len, PTR_STRING, NULL);
        memcpy(*data, query_data, len);
    }
}

static void action_insert(query *_query, void *data)
{
    if (data == NULL)
    {
        if (_query->data == NULL)
            _query->data = avl_create(atomic_create());
        // success
        avl_insert(_query->_tree, _query->id, _query->data);
        dprintf(_query->fd, "OK\n");
    }
    else
    {
        // existent
    }
}

static void action_update(query *_query, void *data)
{
    if (data != NULL)
    {
        if (_query->data == NULL)
            _query->data = avl_create(atomic_create());
        // success
        avl_update(_query->_tree, _query->id, _query->data);
        dprintf(_query->fd, "OK\n");
    }
    else
    {
        // non-existent
    }
}

static void action_delete(query *_query, void *data)
{
    if (data == NULL)
    {
        // non-existent
    }
    else if (ptr_type(data) == PTR_TREE)
    {
        // has subkeys
    }
    else
    {
        // success
        avl_delete(_query->_tree, _query->id);
        dprintf(_query->fd, "OK\n");
    }
}

#if 0
static void action_delete_recursive(query *_query, void *data)
{
    if (data != NULL)
    {
        // success
        avl_delete(_query->_tree, _query->id);
    }
    else
    {
        // non-existent
    }
}
#endif

static void action_find(query *_query, void *data)
{
    if (data == NULL)
    {
        // non-existent
    }
    else if (ptr_type(data) == PTR_TREE)
    {
        // has subkeys
    }
    else
    {
        dprintf(_query->fd, "OK %s\n", (char *) data);
        // success
    }
}

static void *query_loop(void *param)
{
    query *_query = param;

    if (_query->id > 0)
    {
        void *data = avl_find(_query->_tree, _query->id);

        if (queue_size(_query->keys) == 0)
        {
            _query->action(_query, data);
            return NULL;
        }
        else if (data == NULL)
        {
            // no subkeys - inexistent
            return NULL;
        }
        else if (ptr_type(data) != PTR_TREE)
        {
            // no subkeys - it's final data
            return NULL;
        }

        _query->_tree = data;
    }

    key *_key = queue_pop(_query->keys);
    if (_key != NULL)
    {
        _query->id = _key->id;
        if (_query->blocking && queue_size(_query->keys) == 0)
            atomic_blocking(ptr_extra(_query->_tree), query_loop, _query);
        else
            atomic_non_blocking(ptr_extra(_query->_tree), query_loop, _query);
    }

    return NULL;
}

static const struct
{
    char op;
    query _query;
} operation[] = {
        {'+', {.action = action_insert, .blocking = true}},
        {'*', {.action = action_update, .blocking = true}},
        {'-', {.action = action_delete, .blocking = true}},
        {'.', {.action = action_find, .blocking = false}},
};

void query_execute(int fd, char *str)
{
    if (str == NULL || str[0] == '\0' || str[0] == '/' || str[1] != '/')
        return;

    queue *keys = queue_create(NULL);
    char *data = NULL;

    parse(str, &keys, &data);

    for (int i = 0; i < sizeof(operation) / sizeof(operation[0]); i++)
    {
        if (operation[i].op == *str)
        {
            query _q = operation[i]._query;
            _q._tree = master;
            _q.keys = keys;
            _q.fd = fd;
            _q.id = 0;
            _q.data = data;
            query_loop(&_q);
            break;
        }
    }

    queue_destroy(keys);
}

