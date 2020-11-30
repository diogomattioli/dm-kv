#include <string.h>

#include "test.h"
#include "../src/json.c"

#include "pointer.h"
#include "avl_tree.h"

TEST(string)
{
    char *in  = "\"test\"";
    char *out = deserialize_string(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_STRING);
    ASSERT_EQUALS(strcmp(out, "test"), 0);

    ptr_free(out);
}

TEST(true)
{
    char *in  = "true";
    char *out = deserialize_bool_or_null(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_TRUE);

    ptr_free(out);
}

TEST(false)
{
    char *in  = "false";
    char *out = deserialize_bool_or_null(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_FALSE);

    ptr_free(out);
}

TEST(null)
{
    char *in  = "null";
    char *out = deserialize_bool_or_null(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_NULL);

    ptr_free(out);
}

TEST(number)
{
    char *in  = "123";
    char *out = deserialize_number(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(out, "123"), 0);

    ptr_free(out);
}

TEST(fraction)
{
    char *in  = "123.45";
    char *out = deserialize_number(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(out, "123.45"), 0);

    ptr_free(out);
}

TEST(exponent)
{
    char *in  = "123.45e12";
    char *out = deserialize_number(&in);

    ASSERT_EQUALS(ptr_type(out), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(out, "123.45e12"), 0);

    ptr_free(out);
}

TEST(array)
{
    char *in  = "[1,2,3]";
    avl_tree_t *out = deserialize_array(&in);

    const int key_0 = hash_multiplication("0");
    const int key_1 = hash_multiplication("1");
    const int key_2 = hash_multiplication("2");

    void *value_0 = avl_find(out, key_0);
    void *value_1 = avl_find(out, key_1);
    void *value_2 = avl_find(out, key_2);

    ASSERT_EQUALS(ptr_type(out), PTR_TREE);
    ASSERT_EQUALS(ptr_type(value_0), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(value_0, "1"), 0);
    ASSERT_EQUALS(ptr_type(value_1), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(value_1, "2"), 0);
    ASSERT_EQUALS(ptr_type(value_2), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(value_2, "3"), 0);

    avl_destroy(out);
}

TEST(object)
{
    char *in = "{'a': 1, 'b': 'test', 'c': true}";
    avl_tree_t *out = deserialize_object(&in);

    const int key_a = hash_multiplication("a");
    const int key_b = hash_multiplication("b");
    const int key_c = hash_multiplication("c");

    void *value_a = avl_find(out, key_a);
    void *value_b = avl_find(out, key_b);
    void *value_c = avl_find(out, key_c);

    ASSERT_EQUALS(ptr_type(out), PTR_TREE);
    ASSERT_EQUALS(ptr_type(value_a), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(value_a, "1"), 0);
    ASSERT_EQUALS(ptr_type(value_b), PTR_STRING);
    ASSERT_EQUALS(strcmp(value_b, "test"), 0);
    ASSERT_EQUALS(ptr_type(value_c), PTR_TRUE);

    avl_destroy(out);
}

TEST(integration)
{
    char *in = "{'a': 1, 'b': 'test', 'c': true, 'd': false, 'e': null, 'f': [1,2], 'g': {'ga': 'gtest'}}";
    avl_tree_t *out = json_deserialize(&in);

    const int key_a = hash_multiplication("a");
    const int key_b = hash_multiplication("b");
    const int key_c = hash_multiplication("c");
    const int key_d = hash_multiplication("d");
    const int key_e = hash_multiplication("e");
    const int key_f = hash_multiplication("f");
    const int key_g = hash_multiplication("g");

    void *value_a = avl_find(out, key_a);
    void *value_b = avl_find(out, key_b);
    void *value_c = avl_find(out, key_c);
    void *value_d = avl_find(out, key_d);
    void *value_e = avl_find(out, key_e);
    void *value_f = avl_find(out, key_f);
    void *value_g = avl_find(out, key_g);

    ASSERT_EQUALS(ptr_type(out), PTR_TREE);
    ASSERT_EQUALS(ptr_type(value_a), PTR_NUMBER);
    ASSERT_EQUALS(ptr_type(value_b), PTR_STRING);
    ASSERT_EQUALS(ptr_type(value_c), PTR_TRUE);
    ASSERT_EQUALS(ptr_type(value_d), PTR_FALSE);
    ASSERT_EQUALS(ptr_type(value_e), PTR_NULL);
    ASSERT_EQUALS(ptr_type(value_f), PTR_TREE);
    ASSERT_EQUALS(ptr_type(value_g), PTR_TREE);

    ASSERT_EQUALS(strcmp(value_a, "1"), 0);
    ASSERT_EQUALS(strcmp(value_b, "test"), 0);

    const int key_f_0 = hash_multiplication("0");
    const int key_f_1 = hash_multiplication("1");

    void *value_f_0 = avl_find(value_f, key_f_0);
    void *value_f_1 = avl_find(value_f, key_f_1);
    ASSERT_EQUALS(ptr_type(value_f_0), PTR_NUMBER);
    ASSERT_EQUALS(ptr_type(value_f_1), PTR_NUMBER);
    ASSERT_EQUALS(strcmp(value_f_0, "1"), 0);
    ASSERT_EQUALS(strcmp(value_f_1, "2"), 0);

    const int key_g_ga = hash_multiplication("ga");
    void *value_g_ga = avl_find(value_g, key_g_ga);
    ASSERT_EQUALS(ptr_type(value_g_ga), PTR_STRING);
    ASSERT_EQUALS(strcmp(value_g_ga, "gtest"), 0);

    avl_destroy(out);
}

TEST_RUN()
