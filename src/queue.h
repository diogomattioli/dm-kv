#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct _queue queue;

size_t queue_size(queue *_queue);

void queue_push(queue *_queue, void *data);

void *queue_pop(queue *_queue);

queue *queue_create(void *data);

void queue_destroy(queue *_queue);

#endif // __QUEUE_H__
