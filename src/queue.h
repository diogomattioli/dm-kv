#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct queue_t queue_t;

size_t queue_size(queue_t *queue);

void queue_push(queue_t *queue, void *data);

void *queue_pop(queue_t *queue);

queue_t *queue_create(void *data);

void queue_destroy(queue_t *queue);

#endif // __QUEUE_H__
