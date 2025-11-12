#include <stddef.h>
#include "cvec.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

#define CVEC_HOOKS_INIT_SUFFIX custom_allocators
#define CVEC_CUSTOM_ALLOCATORS
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_ALLOCATORS
#define CVEC_HOOKS_INIT_SUFFIX custom_memcpy
#define CVEC_CUSTOM_MEMCPY
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_MEMCPY
#define CVEC_HOOKS_INIT_SUFFIX custom_memmove
#define CVEC_CUSTOM_MEMMOVE
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_MEMMOVE

#define CVEC_HOOKS_INIT_SUFFIX custom_allocators_memcpy
#define CVEC_CUSTOM_ALLOCATORS
#define CVEC_CUSTOM_MEMCPY
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_ALLOCATORS
#undef CVEC_CUSTOM_MEMCPY

#define CVEC_HOOKS_INIT_SUFFIX custom_allocators_memmove
#define CVEC_CUSTOM_ALLOCATORS
#define CVEC_CUSTOM_MEMMOVE
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_ALLOCATORS
#undef CVEC_CUSTOM_MEMMOVE

#define CVEC_HOOKS_INIT_SUFFIX custom_memcpy_memmove
#define CVEC_CUSTOM_MEMCPY
#define CVEC_CUSTOM_MEMMOVE
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_MEMCPY
#undef CVEC_CUSTOM_MEMMOVE

#define CVEC_HOOKS_INIT_SUFFIX custom_allocators_memcpy_memmove
#define CVEC_CUSTOM_ALLOCATORS
#define CVEC_CUSTOM_MEMCPY
#define CVEC_CUSTOM_MEMMOVE
#include "cvec_hooks_init.h"
#undef CVEC_CUSTOM_ALLOCATORS
#undef CVEC_CUSTOM_MEMCPY
#undef CVEC_CUSTOM_MEMMOVE

void test_cvec_hooks_init(void) {
    cvec_hooks_t nullhooks = {0};
    cvec_hooks_t hooks;
    cvec_hooks_init(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT, cvec_hookscmp(&hooks, &nullhooks));
    cvec_hooks_init_custom_allocators(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 3, cvec_hookscmp(&hooks, &nullhooks));
    cvec_hooks_init_custom_memcpy(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 1, cvec_hookscmp(&hooks, &nullhooks));
    cvec_hooks_init_custom_memmove(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 1, cvec_hookscmp(&hooks, &nullhooks));

    cvec_hooks_init_custom_allocators_memcpy(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 4, cvec_hookscmp(&hooks, &nullhooks));
    cvec_hooks_init_custom_allocators_memmove(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 4, cvec_hookscmp(&hooks, &nullhooks));
    cvec_hooks_init_custom_memcpy_memmove(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 2, cvec_hookscmp(&hooks, &nullhooks));

    cvec_hooks_init_custom_allocators_memcpy_memmove(&hooks, CVEC_HOOKS_INIT_OVERWRITE);
    TEST_ASSERT_EQUAL_INT(CVEC_HOOKS_COUNT - 5, cvec_hookscmp(&hooks, &nullhooks));
}

void test_cvec_init(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), NULL);
    TEST_ASSERT_NULL(v.data);
    TEST_ASSERT_EQUAL_size_t(sizeof(int), v.memb_size);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb_cap);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_EQUAL_size_t(ECVEC_NONE, v.error);
}

void test_hooks_raw_free(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), NULL);
    hooks_raw_free(&v, v.data);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    v.hooks.free = NULL;
    hooks_raw_free(&v, v.data);
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_FREE & v.error);
}

void test_hooks_raw_alloc(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), NULL);
    void *data = hooks_raw_alloc(&v, v.memb_size);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    v.hooks.free(data);
    data = NULL;
    v.hooks.alloc = NULL;
    TEST_ASSERT_NULL(hooks_raw_alloc(&v, v.memb_size));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_ALLOC & v.error);
}

void test_hooks_raw_realloc(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), NULL);
    void *data = hooks_raw_realloc(&v, v.memb_size);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    v.hooks.free(data);
    data = NULL;
    v.hooks.realloc = NULL;
    TEST_ASSERT_NULL(hooks_raw_realloc(&v, v.memb_size));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_REALLOC & v.error);
}

void test_hooks_raw_memcpy(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), NULL);
    int *data = v.hooks.alloc(sizeof(int) * 2);
    int src = 5;
    TEST_ASSERT_EQUAL_PTR(data + 1, hooks_raw_memcpy(&v, data + 1, &src, sizeof(int)));
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_INT(src, *(data + 1));
    v.hooks.free(data);
    data = NULL;
    v.hooks.memcpy = NULL;
    TEST_ASSERT_NULL(hooks_raw_memcpy(&v, NULL, NULL, 0));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_MEMCPY & v.error);
}
void test_hooks_raw_memmove(void) {
    cvec_t v;
    cvec_init(&v, sizeof(char), NULL);
    char *data = v.hooks.alloc(10);
    v.hooks.memcpy(data, "test", 4);
    TEST_ASSERT_EQUAL_PTR(data + 3, hooks_raw_memmove(&v, data + 3, data, 4));
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_STRING("test", data + 3);
    v.hooks.free(data);
    data = NULL;
    v.hooks.memmove = NULL;
    TEST_ASSERT_NULL(hooks_raw_memmove(&v, NULL, NULL, 0));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_MEMMOVE & v.error);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_cvec_hooks_init);
    RUN_TEST(test_cvec_init);
    RUN_TEST(test_hooks_raw_free);
    RUN_TEST(test_hooks_raw_alloc);
    RUN_TEST(test_hooks_raw_realloc);
    RUN_TEST(test_hooks_raw_memcpy);
    RUN_TEST(test_hooks_raw_memmove);

    UNITY_END();

    return 0;
}