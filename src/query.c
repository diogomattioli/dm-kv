#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "pointer.h"
#include "avl_tree.h"
#include "queue.h"
#include "atomic.h"

#include "query.h"
#include "hash.h"
#include "json.h"

typedef struct query_t query_t;
typedef struct query_key_t query_key_t;

struct query_t
{
    queue_t *keys;
    avl_tree_t *tree;
    uint32_t id;
    int fd;
    void *data;
    void (*action)(query_t *query, void *data);
    bool blocking;
};

struct query_key_t
{
    uint32_t id;
    char *name;
};

static avl_tree_t *master = NULL;

void query_init()
{
    if (master != NULL)
        return;

    master = avl_create(atomic_create());
}

static void parse(char *query, queue_t **queue, void **data)
{
    char *query_keys = strtok(query, ":");
    char *query_data = strtok(NULL, "\0");

    char *token = strtok(query_keys, "/");
    while ((token = strtok(NULL, "/")) != NULL)
    {
        query_key_t *_key = malloc(sizeof(query_key_t));
        _key->id = hash_multiplication(token);
        _key->name = token;
        queue_push(*queue, _key);
    }

    if (query_data != NULL)
        *data = json_deserialize(&query_data);
}

static void action_insert(query_t *query, void *data)
{
    if (data == NULL)
    {
        if (query->data == NULL)
            query->data = avl_create(atomic_create());
        // success
        avl_insert(query->tree, query->id, query->data);
        dprintf(query->fd, "OK\n");
    }
    else
    {
        // existent
    }
}

static void action_update(query_t *query, void *data)
{
    if (data != NULL)
    {
        if (query->data == NULL)
            query->data = avl_create(atomic_create());
        // success
        avl_update(query->tree, query->id, query->data);
        dprintf(query->fd, "OK\n");
    }
    else
    {
        // non-existent
    }
}

static void action_delete(query_t *query, void *data)
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
        avl_delete(query->tree, query->id);
        dprintf(query->fd, "OK\n");
    }
}

#if 0
static void action_delete_recursive(query_t *query, void *data)
{
    if (data != NULL)
    {
        // success
        avl_delete(query->tree, query->id);
    }
    else
    {
        // non-existent
    }
}
#endif

static void action_find(query_t *query, void *data)
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
        switch (ptr_type(data))
        {
            case PTR_NULL:
                dprintf(query->fd, "OK null\n");
                break;
            case PTR_TRUE:
                dprintf(query->fd, "OK true\n");
                break;
            case PTR_FALSE:
                dprintf(query->fd, "OK false\n");
                break;
            default:
                dprintf(query->fd, "OK %s\n", (char *) data);
                break;
        }
    }
}

static void *query_loop(void *param)
{
    query_t *query = param;

    if (query->id > 0)
    {
        void *data = avl_find(query->tree, query->id);

        if (queue_size(query->keys) == 0)
        {
            query->action(query, data);
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

        query->tree = data;
    }

    query_key_t *_key = queue_pop(query->keys);
    if (_key != NULL)
    {
        query->id = _key->id;
        if (query->blocking && queue_size(query->keys) == 0)
            atomic_blocking(ptr_extra(query->tree), query_loop, query);
        else
            atomic_non_blocking(ptr_extra(query->tree), query_loop, query);
    }

    return NULL;
}

static const struct
{
    char op;
    query_t query;
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

    queue_t *keys = queue_create(NULL);
    void *data = NULL;

    parse(str, &keys, &data);

    for (int i = 0; i < sizeof(operation) / sizeof(operation[0]); i++)
    {
        if (operation[i].op == *str)
        {
            query_t query = operation[i].query;
            query.tree = master;
            query.keys = keys;
            query.fd = fd;
            query.id = 0;
            query.data = data;
            query_loop(&query);
            break;
        }
    }

    queue_destroy(keys);
}

