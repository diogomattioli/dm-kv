#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include <pthread.h>
#include <stdint.h>

#include "avl_tree.h"

typedef struct atomic_t atomic_t;

typedef void *(atomic_func)(void *);

void atomic_inc(pthread_mutex_t *mutex, uint32_t *data);

void atomic_dec(pthread_mutex_t *mutex, uint32_t *data);

void atomic_set(pthread_mutex_t *mutex, uint32_t *data, uint32_t value);

uint32_t atomic_get(pthread_mutex_t *mutex, uint32_t *data);

atomic_t *atomic_create();

void atomic_destroy(atomic_t *atomic);

void *atomic_non_blocking(atomic_t *atomic, atomic_func *func, void *data);

void *atomic_blocking(atomic_t *atomic, atomic_func *func, void *data);

#endif // __ATOMIC_H__