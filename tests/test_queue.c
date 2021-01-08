#include <string.h>

#include "test.h"
#include "../src/queue.c"

static char *item1 = "dummy1";
static char *item2 = "dummy2";
static char *item3 = "dummy3";

TEST(push)
{
    queue_t *queue = queue_create(NULL);

    queue_push(queue, item1);
    ASSERT_EQUALS(queue_size(queue), 1);
    queue_push(queue, item2);
    ASSERT_EQUALS(queue_size(queue), 2);
    queue_push(queue, item3);
    ASSERT_EQUALS(queue_size(queue), 3);

    queue_pop(queue);
    queue_pop(queue);
    queue_pop(queue);

    queue_destroy(queue);
}

TEST(pop)
{
    queue_t *queue = queue_create(NULL);

    queue_push(queue, item1);
    queue_push(queue, item2);
    queue_push(queue, item3);

    char *data = NULL;

    data = queue_pop(queue);
    ASSERT_EQUALS(queue_size(queue), 2);
    ASSERT_EQUALS(strcmp(data, "dummy1"), 0);

    data = queue_pop(queue);
    ASSERT_EQUALS(queue_size(queue), 1);
    ASSERT_EQUALS(strcmp(data, "dummy2"), 0);

    data = queue_pop(queue);
    ASSERT_EQUALS(queue_size(queue), 0);
    ASSERT_EQUALS(strcmp(data, "dummy3"), 0);

    queue_destroy(queue);
}

TEST(size)
{
    queue_t *queue = queue_create(NULL);

    ASSERT_EQUALS(queue_size(queue), 0);

    queue_push(queue, item1);
    ASSERT_EQUALS(queue_size(queue), 1);

    queue_push(queue, item2);
    ASSERT_EQUALS(queue_size(queue), 2);

    queue_pop(queue);
    ASSERT_EQUALS(queue_size(queue), 1);

    queue_pop(queue);
    ASSERT_EQUALS(queue_size(queue), 0);

    queue_destroy(queue);
}

TEST_RUN()
