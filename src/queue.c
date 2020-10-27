#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "pointer.h"

typedef struct _node node;

struct _node
{
    void *data;
    node *prev;
};

struct _queue
{
    size_t size;
    node *front;
    node *back;
};

size_t queue_size(queue *_queue)
{
    return _queue->size;
}

void queue_push(queue *_queue, void *data)
{
    node *_node = malloc(sizeof(node));
    _node->data = data;
    _node->prev = NULL;

    if (_queue->front == NULL)
        _queue->front = _node;
    else
        _queue->back->prev = _node;
    _queue->back = _node;

    _queue->size++;
}

void *queue_pop(queue *_queue)
{
    node *_node = _queue->front;
    if (_node == NULL)
        return NULL;

    _queue->front = _node->prev;
    if (_queue->front == NULL)
        _queue->back = NULL;

    void *data = _node->data;
    free(_node);

    _queue->size--;

    return data;
}

queue *queue_create(void *data)
{
    queue *_queue = ptr_malloc(sizeof(queue), PTR_QUEUE, data);
    memset(_queue, 0, sizeof(queue));
    return _queue;
}

void queue_destroy(queue *_queue)
{
    node *_node = _queue->front;
    while (_node != NULL)
    {
        node *temp = _node;
        _node = _node->prev;

        free(temp->data);
        free(temp);
    }
    ptr_free(_queue);
}
