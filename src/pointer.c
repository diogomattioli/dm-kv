#include <stdlib.h>
#include <stdint.h>

#include "pointer.h"

static int8_t counter_add(void *ptr, int8_t amount)
{
    uint8_t *ptr8 = (uint8_t *) ptr;
    ptr8--;

    uint8_t count = *ptr8 & 0x0f;
    count += amount;

    *ptr8 &= 0xf0;
    *ptr8 |= count & 0x0f;

    return count;
}

void *ptr_malloc(size_t size, int type, void *extra)
{
    size_t esize = (extra != NULL ? sizeof(void *) : 0);

    uint8_t *ptr8 = malloc(size + esize + 1);
    if (ptr8 == NULL)
        return NULL;

    if (esize)
    {
        *((void **) ptr8) = extra;
        ptr8 += esize;
    }

    *ptr8 = 0; // clean
    if (esize)
        *ptr8 |= 1 << 7; // contains extra
    *ptr8 |= (type << 4) & 0x70; // type
    *ptr8 |= 1; // counter

    return ++ptr8;
}

void ptr_free(void *ptr)
{
    if (ptr == NULL)
        return;

    if (counter_add(ptr, -1) > 0)
        return;

    uint8_t *ptr8 = (uint8_t *) ptr;
    ptr8--;

    void (*destroy)(void *) = NULL;
    if (destroy)
        destroy(ptr);

    void *extra = ptr_extra(ptr);
    if (extra)
    {
        free(extra);
        ptr8 -= sizeof(void *);
    }

    free(ptr8);
}

int ptr_type(void *ptr)
{
    if (ptr == NULL)
        return -1;

    uint8_t *ptr8 = (uint8_t *) ptr;
    ptr8--;
    return (*ptr8 >> 4) & 0x07;
}

void *ptr_extra(void *ptr)
{
    if (ptr == NULL)
        return NULL;

    uint8_t *ptr8 = (uint8_t *) ptr;
    ptr8--;

    if ((*ptr8 >> 7 & 1) != 1)
        return NULL;

    ptr8 -= sizeof(void *);

    return *((void **) ptr8);
}

void ptr_share(void *ptr)
{
    if (ptr == NULL)
        return;

    counter_add(ptr, 1);
}
