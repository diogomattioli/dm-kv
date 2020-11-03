#ifndef __POINTER_H__
#define __POINTER_H__

enum
{
    PTR_UNKNOWN,
    PTR_TREE,
    PTR_QUEUE,
    PTR_STRING,

    PTR_NUMBER,
    PTR_TRUE,
    PTR_FALSE,
    PTR_NULL,
};

void *ptr_malloc(size_t size, int type, void *extra);

void ptr_free(void *ptr);

int ptr_type(void *ptr);

void *ptr_extra(void *ptr);

void ptr_share(void *ptr);

#endif // __POINTER_H__
