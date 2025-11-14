#include <stddef.h>
#include "cvec.h"
#include "unity.h"

static cvec_hooks_t g_hooks;

void setUp(void) {
    cvec_hooks_init(&g_hooks, CVEC_HOOKS_INIT_OVERWRITE);
}

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
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_NULL(v.data);
    TEST_ASSERT_EQUAL_size_t(sizeof(int), v.memb_size);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb_cap);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_EQUAL_size_t(ECVEC_NONE, v.error);
}
void test_hooks_raw_free(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    hooks_raw_free(&v, v.data);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    v.hooks.free = NULL;
    hooks_raw_free(&v, v.data);
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_FREE & v.error);
}
void test_hooks_raw_alloc(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
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
    cvec_init(&v, sizeof(int), &g_hooks);
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
    cvec_init(&v, sizeof(int), &g_hooks);
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
    cvec_init(&v, sizeof(char), &g_hooks);
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
void test_hooks_raw_grow(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_size_t(0, hooks_raw_grow(&v, 0, 0, 0));
    TEST_ASSERT_EQUAL_size_t(2, hooks_raw_grow(&v, 2, 1, SIZE_MAX / 4));
    TEST_ASSERT_EQUAL_size_t(0, hooks_raw_grow(&v, 4, 5, SIZE_MAX / 4));
    TEST_ASSERT_GREATER_THAN_size_t(2, hooks_raw_grow(&v, 2, 3, SIZE_MAX / 4));

    v.hooks.grow = NULL;
    TEST_ASSERT_EQUAL_size_t(0, hooks_raw_grow(&v, 1, 2, 2));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_GROW & v.error);
}
void test_hooks_raw_resize(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_INT(0, hooks_raw_resize(&v, 0));
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb | v.nmemb_cap);

    v.memb_size = SIZE_MAX / 2;
    TEST_ASSERT_EQUAL_INT(-1, hooks_raw_resize(&v, 3));
    TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);

    v.error = ECVEC_NONE;
    v.memb_size = sizeof(int);
    TEST_ASSERT_EQUAL_INT(0, hooks_raw_resize(&v, 3));
    TEST_ASSERT_NOT_NULL(v.data);
    TEST_ASSERT_GREATER_THAN_size_t(0, v.nmemb_cap);

    v.nmemb = v.nmemb_cap;
    TEST_ASSERT_EQUAL_INT(0, hooks_raw_resize(&v, v.nmemb_cap - 2));
    TEST_ASSERT_EQUAL_size_t(v.nmemb, v.nmemb_cap);

    TEST_ASSERT_EQUAL_INT(0, hooks_raw_resize(&v, 0));
    TEST_ASSERT_NULL(v.data);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb_cap | v.nmemb);

    v.hooks.resize = NULL;
    TEST_ASSERT_EQUAL_INT(-1, hooks_raw_resize(&v, 0));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_RESIZE & v.error);
}
void test_hooks_raw_push(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_INT(0, hooks_raw_push(&v, &(int){25}));
    TEST_ASSERT_EQUAL_INT(25, *(int *)v.data);
    TEST_ASSERT_EQUAL_size_t(1, v.nmemb);
    v.hooks.free(v.data);

    v.nmemb = SIZE_MAX;
    TEST_ASSERT_EQUAL_INT(-1, hooks_raw_push(&v, &(int){2}));
    TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);

    v.nmemb = SIZE_MAX - sizeof(int) + 1;
    TEST_ASSERT_EQUAL_INT(-1, hooks_raw_push(&v, &(int){2}));

    v.hooks.push = NULL;
    TEST_ASSERT_EQUAL_INT(-1, hooks_raw_push(&v, &(int){2}));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_PUSH & v.error);
}
void test_hooks_raw_pushn(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);

    TEST_ASSERT_EQUAL_INT(0, hooks_raw_pushn(&v, &(int){42}, 0));
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    TEST_ASSERT_EQUAL_INT(0, hooks_raw_pushn(&v, &(int){7}, 5));
    TEST_ASSERT_EQUAL_size_t(5, v.nmemb);

    int *data = (int *)v.data;
    for (size_t i = 0; i < v.nmemb; ++i) { TEST_ASSERT_EQUAL_INT(7, data[i]); }

    v.hooks.free(v.data);
    v.data = NULL;
    v.nmemb = v.nmemb_cap = 0;

    cvec_init(&v, sizeof(int), &g_hooks);
    v.error = ECVEC_NONE;
    v.hooks.pushn = NULL;

    TEST_ASSERT_EQUAL_INT(-1, hooks_raw_pushn(&v, &(int){1}, 3));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_PUSHN & v.error);

    TEST_ASSERT_NULL(v.data);
}
void test_hooks_raw_insert(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    /* Build initial vector [1, 3] */
    TEST_ASSERT_EQUAL_INT(0, hooks_raw_push(&v, &(int){1}));
    TEST_ASSERT_EQUAL_INT(0, hooks_raw_push(&v, &(int){3}));
    TEST_ASSERT_EQUAL_size_t(2, v.nmemb);

    int *arr = NULL;

    /* ---- Subtest 1: insert in the middle ---- */
    int value2 = 2;
    int *p = (int *)hooks_raw_insert(&v, &value2, 1);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(2, *p);
    TEST_ASSERT_EQUAL_size_t(3, v.nmemb);

    arr = (int *)v.data;
    TEST_ASSERT_EQUAL_INT(1, arr[0]);
    TEST_ASSERT_EQUAL_INT(2, arr[1]);
    TEST_ASSERT_EQUAL_INT(3, arr[2]);
    TEST_ASSERT_EQUAL_PTR(&arr[1], p);

    /* ---- Subtest 2: insert at the front ---- */
    int value0 = 0;
    p = (int *)hooks_raw_insert(&v, &value0, 0);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(0, *p);
    TEST_ASSERT_EQUAL_size_t(4, v.nmemb);

    arr = (int *)v.data;
    TEST_ASSERT_EQUAL_INT(0, arr[0]);
    TEST_ASSERT_EQUAL_INT(1, arr[1]);
    TEST_ASSERT_EQUAL_INT(2, arr[2]);
    TEST_ASSERT_EQUAL_INT(3, arr[3]);
    TEST_ASSERT_EQUAL_PTR(&arr[0], p);

    /* ---- Subtest 3: insert at the back (index == nmemb) ---- */
    int value4 = 4;
    p = (int *)hooks_raw_insert(&v, &value4, v.nmemb);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(4, *p);
    TEST_ASSERT_EQUAL_size_t(5, v.nmemb);

    arr = (int *)v.data;
    TEST_ASSERT_EQUAL_INT(0, arr[0]);
    TEST_ASSERT_EQUAL_INT(1, arr[1]);
    TEST_ASSERT_EQUAL_INT(2, arr[2]);
    TEST_ASSERT_EQUAL_INT(3, arr[3]);
    TEST_ASSERT_EQUAL_INT(4, arr[4]);
    TEST_ASSERT_EQUAL_PTR(&arr[4], p);

    /* ---- Subtest 4: invalid index (> nmemb) ---- */
    v.error = ECVEC_NONE;
    TEST_ASSERT_NULL(hooks_raw_insert(&v, &value0, v.nmemb + 1));
    TEST_ASSERT_TRUE(ECVEC_INVALID_INDEX & v.error);

    /* ---- Subtest 5: missing hook ---- */
    v.error = ECVEC_NONE;
    v.hooks.insert = NULL;
    TEST_ASSERT_NULL(hooks_raw_insert(&v, &value0, 0));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_INSERT & v.error);

    /* clean up */
    g_hooks.free(v.data);
}
void test_hooks_raw_erase(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    /* Build initial vector [10, 11, 12, 13, 14] */
    for (int i = 0; i < 5; ++i) { TEST_ASSERT_EQUAL_INT(0, hooks_raw_push(&v, &(int){10 + i})); }
    TEST_ASSERT_EQUAL_size_t(5, v.nmemb);

    int *arr = NULL;

    /* ---- Subtest 1: erase a middle range [1, 3] -> [10, 14] ---- */
    v.error = ECVEC_NONE;
    int *p = (int *)hooks_raw_erase(&v, 1, 3);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_size_t(2, v.nmemb);

    arr = (int *)v.data;
    TEST_ASSERT_EQUAL_INT(10, arr[0]);
    TEST_ASSERT_EQUAL_INT(14, arr[1]);

    /* returned pointer should point to the first element after the erased range,
       i.e., the element now at index 1 */
    TEST_ASSERT_EQUAL_PTR(&arr[1], p);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    /* ---- Subtest 2: erase a suffix [1, 10] -> [10] ---- */
    v.error = ECVEC_NONE;
    p = (int *)hooks_raw_erase(&v, 1, 10);
    TEST_ASSERT_NULL(p); /* erasing suffix returns NULL */
    TEST_ASSERT_EQUAL_size_t(1, v.nmemb);
    arr = (int *)v.data;
    TEST_ASSERT_EQUAL_INT(10, arr[0]);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    /* ---- Subtest 3: invalid first index (>= nmemb) ---- */
    v.error = ECVEC_NONE;
    TEST_ASSERT_NULL(hooks_raw_erase(&v, 5, 6));
    TEST_ASSERT_TRUE(ECVEC_INVALID_INDEX & v.error);

    /* ---- Subtest 4: erase to empty, then erase on empty ---- */
    v.error = ECVEC_NONE;
    /* erase the only remaining element [0, 0] -> [] */
    p = (int *)hooks_raw_erase(&v, 0, 0);
    TEST_ASSERT_NULL(p);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);

    /* now vector is empty; your default_cvec_erase returns vec->data directly */
    v.error = ECVEC_NONE;
    void *saved_data = v.data;
    p = (int *)hooks_raw_erase(&v, 0, 5);
    TEST_ASSERT_EQUAL_PTR(saved_data, p);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    /* ---- Subtest 5: missing hook ---- */
    v.error = ECVEC_NONE;
    v.hooks.erase = NULL;
    TEST_ASSERT_NULL(hooks_raw_erase(&v, 0, 0));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_ERASE & v.error);

    /* clean up */
    g_hooks.free(v.data);
}
void test_ALL_hooks_raw(void) {
    RUN_TEST(test_hooks_raw_free);
    RUN_TEST(test_hooks_raw_alloc);
    RUN_TEST(test_hooks_raw_realloc);
    RUN_TEST(test_hooks_raw_memcpy);
    RUN_TEST(test_hooks_raw_memmove);
    RUN_TEST(test_hooks_raw_grow);
    RUN_TEST(test_hooks_raw_resize);
    RUN_TEST(test_hooks_raw_push);
    RUN_TEST(test_hooks_raw_pushn);
    RUN_TEST(test_hooks_raw_insert);
    RUN_TEST(test_hooks_raw_erase);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_cvec_hooks_init);
    RUN_TEST(test_cvec_init);
    test_ALL_hooks_raw();

    UNITY_END();

    return 0;
}