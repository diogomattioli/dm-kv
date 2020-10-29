#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "pointer.h"

typedef struct queue_node_t queue_node_t;

struct queue_node_t
{
    void *data;
    queue_node_t *prev;
};

struct queue_t
{
    size_t size;
    queue_node_t *front;
    queue_node_t *back;
};

size_t queue_size(queue_t *queue)
{
    return queue->size;
}

void queue_push(queue_t *queue, void *data)
{
    queue_node_t *node = malloc(sizeof(queue_node_t));
    node->data = data;
    node->prev = NULL;

    if (queue->front == NULL)
        queue->front = node;
    else
        queue->back->prev = node;
    queue->back = node;

    queue->size++;
}

void *queue_pop(queue_t *queue)
{
    queue_node_t *node = queue->front;
    if (node == NULL)
        return NULL;

    queue->front = node->prev;
    if (queue->front == NULL)
        queue->back = NULL;

    void *data = node->data;
    free(node);

    queue->size--;

    return data;
}

queue_t *queue_create(void *data)
{
    queue_t *queue = ptr_malloc(sizeof(queue_t), PTR_QUEUE, data);
    memset(queue, 0, sizeof(queue_t));
    return queue;
}

void queue_destroy(queue_t *queue)
{
    queue_node_t *node = queue->front;
    while (node != NULL)
    {
        queue_node_t *temp = node;
        node = node->prev;

        free(temp->data);
        free(temp);
    }
    ptr_free(queue);
}
