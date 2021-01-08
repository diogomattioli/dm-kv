#include <stdio.h>
#include <string.h>

#include "test.h"
#include "../src/pointer.c"

TEST(type)
{
    void *ptr0 = ptr_malloc(1, PTR_NUMBER, NULL);
    void *ptr1 = ptr_malloc(1, PTR_UNKNOWN, NULL);
    void *ptr2 = ptr_malloc(1, PTR_FALSE, NULL);
    void *ptr3 = ptr_malloc(1, PTR_NULL, NULL);
    void *ptr4 = ptr_malloc(1, PTR_QUEUE, NULL);
    void *ptr5 = ptr_malloc(1, PTR_STRING, NULL);
    void *ptr6 = ptr_malloc(1, PTR_TREE, NULL);
    void *ptr7 = ptr_malloc(1, PTR_TRUE, NULL);

    ASSERT_EQUALS(ptr_type(ptr0), PTR_NUMBER);
    ASSERT_EQUALS(ptr_type(ptr1), PTR_UNKNOWN);
    ASSERT_EQUALS(ptr_type(ptr2), PTR_FALSE);
    ASSERT_EQUALS(ptr_type(ptr3), PTR_NULL);
    ASSERT_EQUALS(ptr_type(ptr4), PTR_QUEUE);
    ASSERT_EQUALS(ptr_type(ptr5), PTR_STRING);
    ASSERT_EQUALS(ptr_type(ptr6), PTR_TREE);
    ASSERT_EQUALS(ptr_type(ptr7), PTR_TRUE);

    ptr_free(ptr0);
    ptr_free(ptr1);
    ptr_free(ptr2);
    ptr_free(ptr3);
    ptr_free(ptr4);
    ptr_free(ptr5);
    ptr_free(ptr6);
    ptr_free(ptr7);
}

TEST(extra)
{
    char *extra = malloc(sizeof(char) * 6);
    sprintf(extra, "extra");

    void *ptr0 = ptr_malloc(1, PTR_UNKNOWN, NULL);
    void *ptr1 = ptr_malloc(1, PTR_UNKNOWN, extra);

    ASSERT_EQUALS(ptr_extra(ptr0), NULL);
    ASSERT_EQUALS(strcmp(ptr_extra(ptr1), "extra"), 0);

    ptr_free(ptr0);
    ptr_free(ptr1);
}

TEST(share)
{
    void *ptr = ptr_malloc(1, PTR_UNKNOWN, NULL);

    ASSERT_EQUALS(counter_add(ptr, 0), 1);
    ptr_share(ptr);
    ASSERT_EQUALS(counter_add(ptr, 0), 2);
    ptr_share(ptr);
    ASSERT_EQUALS(counter_add(ptr, 0), 3);
    ptr_free(ptr);
    ASSERT_EQUALS(counter_add(ptr, 0), 2);
    ptr_free(ptr);
    ASSERT_EQUALS(counter_add(ptr, 0), 1);

    ptr_free(ptr);
}

TEST_RUN()
