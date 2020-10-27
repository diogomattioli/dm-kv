#include <stdlib.h>

#include "atomic.h"

struct _atomic
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex_counter;
    uint32_t counter;
};

void atomic_inc(pthread_mutex_t *mutex, uint32_t *data)
{
    pthread_mutex_lock(mutex);
    (*data)++;
    pthread_mutex_unlock(mutex);
}

void atomic_dec(pthread_mutex_t *mutex, uint32_t *data)
{
    pthread_mutex_lock(mutex);
    (*data)--;
    pthread_mutex_unlock(mutex);
}

void atomic_set(pthread_mutex_t *mutex, uint32_t *data, uint32_t value)
{
    pthread_mutex_lock(mutex);
    *data = value;
    pthread_mutex_unlock(mutex);
}

uint32_t atomic_get(pthread_mutex_t *mutex, uint32_t *data)
{
    pthread_mutex_lock(mutex);
    uint32_t value = *data;
    pthread_mutex_unlock(mutex);
    return value;
}

atomic *atomic_create()
{
    atomic *_atomic = malloc(sizeof(struct _atomic));
    if (_atomic == NULL)
        return NULL;

    pthread_cond_init(&_atomic->cond, NULL);
    pthread_mutex_init(&_atomic->mutex, NULL);
    pthread_mutex_init(&_atomic->mutex_counter, NULL);
    _atomic->counter = 0;

    return _atomic;
}

void atomic_destroy(atomic *_atomic)
{
    pthread_cond_destroy(&_atomic->cond);
    pthread_mutex_destroy(&_atomic->mutex);
    pthread_mutex_destroy(&_atomic->mutex_counter);
    free(_atomic);
}

void *atomic_non_blocking(atomic *_atomic, atomic_func *func, void *data)
{
    pthread_mutex_lock(&_atomic->mutex);
    atomic_inc(&_atomic->mutex_counter, &_atomic->counter);
    pthread_mutex_unlock(&_atomic->mutex);

    void *ret = func(data);

    atomic_dec(&_atomic->mutex_counter, &_atomic->counter);
    pthread_cond_signal(&_atomic->cond);

    return ret;
}

void *atomic_blocking(atomic *_atomic, atomic_func *func, void *data)
{
    pthread_mutex_lock(&_atomic->mutex);

    while (atomic_get(&_atomic->mutex_counter, &_atomic->counter) > 0)
        pthread_cond_wait(&_atomic->cond, &_atomic->mutex);

    void *ret = func(data);

    pthread_mutex_unlock(&_atomic->mutex);

    return ret;
}
