#include <stdlib.h>

#include "atomic.h"

struct atomic_t
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

atomic_t *atomic_create()
{
    atomic_t *atomic = malloc(sizeof(atomic_t));
    if (atomic == NULL)
        return NULL;

    pthread_cond_init(&atomic->cond, NULL);
    pthread_mutex_init(&atomic->mutex, NULL);
    pthread_mutex_init(&atomic->mutex_counter, NULL);
    atomic->counter = 0;

    return atomic;
}

void atomic_destroy(atomic_t *atomic)
{
    pthread_cond_destroy(&atomic->cond);
    pthread_mutex_destroy(&atomic->mutex);
    pthread_mutex_destroy(&atomic->mutex_counter);
    free(atomic);
}

void *atomic_non_blocking(atomic_t *atomic, atomic_func *func, void *data)
{
    pthread_mutex_lock(&atomic->mutex);
    atomic_inc(&atomic->mutex_counter, &atomic->counter);
    pthread_mutex_unlock(&atomic->mutex);

    void *ret = func(data);

    atomic_dec(&atomic->mutex_counter, &atomic->counter);
    pthread_cond_signal(&atomic->cond);

    return ret;
}

void *atomic_blocking(atomic_t *atomic, atomic_func *func, void *data)
{
    pthread_mutex_lock(&atomic->mutex);

    while (atomic_get(&atomic->mutex_counter, &atomic->counter) > 0)
        pthread_cond_wait(&atomic->cond, &atomic->mutex);

    void *ret = func(data);

    pthread_mutex_unlock(&atomic->mutex);

    return ret;
}
