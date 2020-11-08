#ifndef __TEST_H__
#define __TEST_H__

#include <stdlib.h>
#include <stdbool.h>

bool __test_error__;
void (*__test_cases__[25])() = {NULL};

#define TEST_RUN_1(y) __test_cases__[0] = __test_0__;
#define TEST_RUN_2(y)               \
    __test_cases__[1] = __test_1__; \
    TEST_RUN_1(y)
#define TEST_RUN_3(y)               \
    __test_cases__[2] = __test_2__; \
    TEST_RUN_2(y)
#define TEST_RUN_4(y)               \
    __test_cases__[3] = __test_3__; \
    TEST_RUN_3(y)
#define TEST_RUN_5(y)               \
    __test_cases__[4] = __test_4__; \
    TEST_RUN_4(y)
#define TEST_RUN_6(y)               \
    __test_cases__[5] = __test_5__; \
    TEST_RUN_5(y)
#define TEST_RUN_7(y)               \
    __test_cases__[6] = __test_6__; \
    TEST_RUN_6(y)
#define TEST_RUN_8(y)               \
    __test_cases__[7] = __test_7__; \
    TEST_RUN_7(y)
#define TEST_RUN_9(y)               \
    __test_cases__[8] = __test_8__; \
    TEST_RUN_8(y)
#define TEST_RUN_10(y)              \
    __test_cases__[9] = __test_9__; \
    TEST_RUN_9(y)
#define TEST_RUN_11(y)                \
    __test_cases__[10] = __test_10__; \
    TEST_RUN_10(y)
#define TEST_RUN_12(y)                \
    __test_cases__[11] = __test_11__; \
    TEST_RUN_11(y)
#define TEST_RUN_13(y)                \
    __test_cases__[12] = __test_12__; \
    TEST_RUN_12(y)
#define TEST_RUN_14(y)                \
    __test_cases__[13] = __test_13__; \
    TEST_RUN_13(y)
#define TEST_RUN_15(y)                \
    __test_cases__[14] = __test_14__; \
    TEST_RUN_14(y)
#define TEST_RUN_16(y)                \
    __test_cases__[15] = __test_15__; \
    TEST_RUN_15(y)
#define TEST_RUN_17(y)                \
    __test_cases__[16] = __test_16__; \
    TEST_RUN_16(y)
#define TEST_RUN_18(y)                \
    __test_cases__[17] = __test_17__; \
    TEST_RUN_17(y)
#define TEST_RUN_19(y)                \
    __test_cases__[18] = __test_18__; \
    TEST_RUN_18(y)
#define TEST_RUN_20(y)                \
    __test_cases__[19] = __test_19__; \
    TEST_RUN_19(y)
#define TEST_RUN_21(y)                \
    __test_cases__[20] = __test_20__; \
    TEST_RUN_20(y)
#define TEST_RUN_22(y)                \
    __test_cases__[21] = __test_21__; \
    TEST_RUN_21(y)
#define TEST_RUN_23(y)                \
    __test_cases__[22] = __test_22__; \
    TEST_RUN_22(y)
#define TEST_RUN_24(y)                \
    __test_cases__[23] = __test_23__; \
    TEST_RUN_23(y)
#define TEST_RUN_25(y)                \
    __test_cases__[24] = __test_24__; \
    TEST_RUN_24(y)
#define TEST_RUN_R1(y) TEST_RUN_##y(y)
#define TEST_RUN_R2(y) TEST_RUN_R1(y)
#define TEST_RUN_R3() TEST_RUN_R2(__COUNTER__)
#define TEST_RUN()                         \
    void __test_initialize__()             \
    {                                      \
        TEST_RUN_R3();                     \
    }

#define TEST_1(name, number, line)         \
    void __test_##number##_##line##__();   \
    void __test_##number##__()             \
    {                                      \
        __test_##number##_##line##__();    \
    }                                      \
    void __test_##number##_##line##__()
#define TEST_2(name, number, line) TEST_1(name, number, line)
#define TEST(name) TEST_2(name, __COUNTER__, __LINE__)

#define __ASSERT_ERROR__                                  \
    {                                                     \
        __test_error__ = true;                            \
        return;                                           \
    }
#define ASSERT_EQUALS(x, y) \
    if (!(x == y))          \
    __ASSERT_ERROR__
#define ASSERT_NOT_EQUALS(x, y) \
    if (!(x != y))              \
    __ASSERT_ERROR__
#define ASSERT_NULL(x) \
    if (!(x == NULL))  \
    __ASSERT_ERROR__
#define ASSERT_NOT_NULL(x) \
    if (!(x != NULL))      \
    __ASSERT_ERROR__
#define ASSERT_TRUE(x) \
    if (!(x))          \
    __ASSERT_ERROR__
#define ASSERT_FALSE(x) \
    if (!(!x))          \
    __ASSERT_ERROR__

void __test_initialize__();

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    size_t n = atoi(argv[1]);

    __test_initialize__();
    if (n > sizeof(__test_cases__) / sizeof(__test_cases__[0]))
        return 1;
    __test_cases__[n]();
    return __test_error__ ? 1 : 0;
}

#endif //__TEST_H__
