////////////////////////
//
//  Created: Sun Nov 17 2024
//  File: main.c
//
////////////////////////

#include "kernel/lib/string.h"
#include "unity.h"

#define UNUSED(x) ((void)x)

void setUp(void) {}
void tearDown(void) {}

void test_cos_strlen(void)
{
    const char str[13] = "Hello World!";

    TEST_ASSERT_EQUAL_INT(0, cos_strlen(""));
    TEST_ASSERT_EQUAL_INT(12, cos_strlen(str));
}

void test_cos_strcmp(void)
{
    const char str[13] = "Hello World!";
    const char str2[15] = "Goodbye World!";

    TEST_ASSERT_EQUAL_INT(1, cos_strcmp(str, str2));
    TEST_ASSERT_EQUAL_INT(-1, cos_strcmp(str2, str));
    TEST_ASSERT_EQUAL_INT(0, cos_strcmp(str, str));
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    if (TEST_PROTECT()) {
        RUN_TEST(test_cos_strlen);
        RUN_TEST(test_cos_strcmp);
    }
    UNUSED(argc);
    UNUSED(argv);
    return UNITY_END();
}
