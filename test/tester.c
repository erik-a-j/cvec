#include <stdarg.h>
#include <stddef.h>
#include <string.h>
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

void test_default_cvec_grow(void) {
    TEST_ASSERT_EQUAL_size_t(0, default_cvec_grow(0, 0, 0));
    TEST_ASSERT_EQUAL_size_t(2, default_cvec_grow(2, 1, SIZE_MAX / 4));
    TEST_ASSERT_EQUAL_size_t(0, default_cvec_grow(4, 5, SIZE_MAX / 4));
    TEST_ASSERT_GREATER_THAN_size_t(2, default_cvec_grow(2, 3, SIZE_MAX / 4));
}
void test_default_cvec_resize(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_INT(0, default_cvec_resize(&v, 0));
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb | v.nmemb_cap);

    v.memb_size = SIZE_MAX / 2;
    TEST_ASSERT_EQUAL_INT(-1, default_cvec_resize(&v, 3));
    TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);

    v.error = ECVEC_NONE;
    v.memb_size = sizeof(int);
    TEST_ASSERT_EQUAL_INT(0, default_cvec_resize(&v, 3));
    TEST_ASSERT_NOT_NULL(v.data);
    TEST_ASSERT_GREATER_THAN_size_t(0, v.nmemb_cap);

    v.nmemb = v.nmemb_cap;
    TEST_ASSERT_EQUAL_INT(0, default_cvec_resize(&v, v.nmemb_cap - 2));
    TEST_ASSERT_EQUAL_size_t(v.nmemb, v.nmemb_cap);

    TEST_ASSERT_EQUAL_INT(0, default_cvec_resize(&v, 0));
    TEST_ASSERT_NULL(v.data);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb_cap | v.nmemb);
}
void test_default_cvec_push(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_INT(0, default_cvec_push(&v, &(int){25}));
    TEST_ASSERT_EQUAL_INT(25, *(int *)v.data);
    TEST_ASSERT_EQUAL_size_t(1, v.nmemb);
    v.hooks.free(v.data);

    v.nmemb = SIZE_MAX;
    TEST_ASSERT_EQUAL_INT(-1, default_cvec_push(&v, &(int){2}));
    TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);

    v.nmemb = SIZE_MAX - sizeof(int) + 1;
    TEST_ASSERT_EQUAL_INT(-1, default_cvec_push(&v, &(int){2}));
}
void test_default_cvec_pushn(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);

    TEST_ASSERT_EQUAL_INT(0, default_cvec_pushn(&v, &(int){42}, 0));
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);

    TEST_ASSERT_EQUAL_INT(0, default_cvec_pushn(&v, &(int){7}, 5));
    TEST_ASSERT_EQUAL_size_t(5, v.nmemb);

    int *data = (int *)v.data;
    for (size_t i = 0; i < v.nmemb; ++i) { TEST_ASSERT_EQUAL_INT(7, data[i]); }

    v.hooks.free(v.data);
}

__attribute__((format(printf, 2, 3))) int default_cvec_vpushf_wrapper(cvec_t *vec, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int r = default_cvec_vpushf(vec, fmt, ap);
    va_end(ap);
    return r;
}
void test_default_cvec_vpushf(void) {
    cvec_t v;
    cvec_init(&v, sizeof(char), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_NULL(v.data);

    /* ---- Subtest 1: simple format into empty vector ---- */
    {
        TEST_ASSERT_EQUAL_INT(0, default_cvec_vpushf_wrapper(&v, "Hello %s", "world"));
        TEST_ASSERT_NOT_NULL(v.data);

        char *buf = (char *)v.data;
        /* nmemb should equal strlen("Hello world") */
        TEST_ASSERT_EQUAL_size_t(strlen("Hello world"), v.nmemb);
        TEST_ASSERT_EQUAL_STRING_LEN("Hello world", buf, v.nmemb);
        /* internal NUL: buf[nmemb] should be '\0' */
        TEST_ASSERT_EQUAL_CHAR('\0', buf[v.nmemb]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 2: append another formatted chunk ---- */
    {
        size_t old_nmemb = v.nmemb;
        TEST_ASSERT_EQUAL_INT(0, default_cvec_vpushf_wrapper(&v, " %d", 42));
        TEST_ASSERT_GREATER_THAN_size_t(old_nmemb, v.nmemb);

        char *buf = (char *)v.data;
        /* Expect "Hello world 42" as the concatenation */
        const char *expected = "Hello world 42";
        TEST_ASSERT_EQUAL_size_t(strlen(expected), v.nmemb);
        TEST_ASSERT_EQUAL_STRING_LEN(expected, buf, v.nmemb);
        TEST_ASSERT_EQUAL_CHAR('\0', buf[v.nmemb]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 3: zero-length output (e.g. empty format) ---- */
    {
        size_t old_nmemb = v.nmemb;
        TEST_ASSERT_EQUAL_INT(0, default_cvec_vpushf_wrapper(&v, ""));
        TEST_ASSERT_EQUAL_size_t(old_nmemb, v.nmemb);

        char *buf = (char *)v.data;
        /* contents should still be the same string, still NUL-terminated */
        const char *expected = "Hello world 42";
        TEST_ASSERT_EQUAL_STRING_LEN(expected, buf, v.nmemb);
        TEST_ASSERT_EQUAL_CHAR('\0', buf[v.nmemb]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 4: overflow in nmemb + needed + 1 ---- */
    {
        v.error = ECVEC_NONE;

        /* Force nmemb near SIZE_MAX to trigger want < start */
        v.nmemb = SIZE_MAX;
        /* we don't care about actual string content, just overflow handling */
        TEST_ASSERT_EQUAL_INT(-1, default_cvec_vpushf_wrapper(&v, "x"));
        TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);
        /* nmemb must not have changed */
        TEST_ASSERT_EQUAL_size_t(SIZE_MAX, v.nmemb);
    }

    /* cleanup buffer using hooks to avoid leaks under sanitizers */
    if (v.data) {
        g_hooks.free(v.data);
    }
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
void test_hooks_raw_vpushf(void) {
    cvec_t v;
    cvec_init(&v, sizeof(char), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);
    TEST_ASSERT_NULL(v.data);

    /* ---- Subtest 1: simple format into empty vector ---- */
    {
        TEST_ASSERT_EQUAL_INT(0, cvec_pushf(&v, "Hello %s", "world"));
        TEST_ASSERT_NOT_NULL(v.data);

        char *buf = (char *)v.data;
        /* nmemb should equal strlen("Hello world") */
        TEST_ASSERT_EQUAL_size_t(strlen("Hello world"), v.nmemb);
        TEST_ASSERT_EQUAL_STRING_LEN("Hello world", buf, v.nmemb);
        /* internal NUL: buf[nmemb] should be '\0' */
        TEST_ASSERT_EQUAL_CHAR('\0', buf[v.nmemb]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 2: append another formatted chunk ---- */
    {
        size_t old_nmemb = v.nmemb;
        TEST_ASSERT_EQUAL_INT(0, cvec_pushf(&v, " %d", 42));
        TEST_ASSERT_GREATER_THAN_size_t(old_nmemb, v.nmemb);

        char *buf = (char *)v.data;
        /* Expect "Hello world 42" as the concatenation */
        const char *expected = "Hello world 42";
        TEST_ASSERT_EQUAL_size_t(strlen(expected), v.nmemb);
        TEST_ASSERT_EQUAL_STRING_LEN(expected, buf, v.nmemb);
        TEST_ASSERT_EQUAL_CHAR('\0', buf[v.nmemb]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 3: zero-length output (e.g. empty format) ---- */
    {
        size_t old_nmemb = v.nmemb;
        TEST_ASSERT_EQUAL_INT(0, cvec_pushf(&v, ""));
        TEST_ASSERT_EQUAL_size_t(old_nmemb, v.nmemb);

        char *buf = (char *)v.data;
        /* contents should still be the same string, still NUL-terminated */
        const char *expected = "Hello world 42";
        TEST_ASSERT_EQUAL_STRING_LEN(expected, buf, v.nmemb);
        TEST_ASSERT_EQUAL_CHAR('\0', buf[v.nmemb]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 4: overflow in nmemb + needed + 1 ---- */
    {
        v.error = ECVEC_NONE;

        /* Force nmemb near SIZE_MAX to trigger want < start */
        v.nmemb = SIZE_MAX;
        /* we don't care about actual string content, just overflow handling */
        TEST_ASSERT_EQUAL_INT(-1, cvec_pushf(&v, "x"));
        TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);
        /* nmemb must not have changed */
        TEST_ASSERT_EQUAL_size_t(SIZE_MAX, v.nmemb);
    }

    v.hooks.vpushf = NULL;
    TEST_ASSERT_EQUAL_INT(-1, cvec_pushf(&v, "x"));
    TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_VPUSHF & v.error);

    /* cleanup buffer using hooks to avoid leaks under sanitizers */
    if (v.data) {
        g_hooks.free(v.data);
    }
}
void test_hooks_raw_append(void) {
    cvec_t v;
    cvec_init(&v, sizeof(int), &g_hooks);
    TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    TEST_ASSERT_EQUAL_size_t(0, v.nmemb);

    /* ---- Subtest 1: append into empty vector ---- */
    {
        int src1[] = {1, 2, 3};

        /* Append 3 elements: expect [1, 2, 3] */
        TEST_ASSERT_EQUAL_INT(0, hooks_raw_append(&v, src1, 3));
        TEST_ASSERT_EQUAL_size_t(3, v.nmemb);

        int *data = (int *)v.data;
        TEST_ASSERT_NOT_NULL(data);
        TEST_ASSERT_EQUAL_INT(1, data[0]);
        TEST_ASSERT_EQUAL_INT(2, data[1]);
        TEST_ASSERT_EQUAL_INT(3, data[2]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 2: append onto existing data ---- */
    {
        int src2[] = {4, 5};

        size_t old_nmemb = v.nmemb;
        TEST_ASSERT_EQUAL_INT(0, hooks_raw_append(&v, src2, 2));
        TEST_ASSERT_EQUAL_size_t(old_nmemb + 2, v.nmemb);

        int *data = (int *)v.data;
        /* now expect [1, 2, 3, 4, 5] */
        TEST_ASSERT_EQUAL_INT(1, data[0]);
        TEST_ASSERT_EQUAL_INT(2, data[1]);
        TEST_ASSERT_EQUAL_INT(3, data[2]);
        TEST_ASSERT_EQUAL_INT(4, data[3]);
        TEST_ASSERT_EQUAL_INT(5, data[4]);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 3: count == 0 is a no-op ---- */
    {
        int dummy[] = {42, 43};

        v.error = ECVEC_NONE;
        size_t old_nmemb = v.nmemb;
        void *old_data = v.data;

        TEST_ASSERT_EQUAL_INT(0, hooks_raw_append(&v, dummy, 0));
        TEST_ASSERT_EQUAL_size_t(old_nmemb, v.nmemb);
        TEST_ASSERT_EQUAL_PTR(old_data, v.data);
        TEST_ASSERT_EQUAL_UINT32(ECVEC_NONE, v.error);
    }

    /* ---- Subtest 4: overflow in count * memb_size ---- */
    {
        v.error = ECVEC_NONE;

        /* Force memb_size so that count * memb_size overflows the check */
        v.memb_size = SIZE_MAX / 2;
        size_t old_nmemb = v.nmemb;
        size_t old_cap = v.nmemb_cap;
        void *old_data = v.data;

        int dummy[] = {1, 2, 3};
        /* count == 3, SIZE_MAX / memb_size will be <= 2, so count > SIZE_MAX/memb_size */
        TEST_ASSERT_EQUAL_INT(-1, hooks_raw_append(&v, dummy, 3));
        TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);

        /* vector logical state should not have changed */
        TEST_ASSERT_EQUAL_size_t(old_nmemb, v.nmemb);
        TEST_ASSERT_EQUAL_size_t(old_cap, v.nmemb_cap);
        TEST_ASSERT_EQUAL_PTR(old_data, v.data);
    }

    /* ---- Subtest 5: overflow in nmemb + count (want < start) ---- */
    {
        v.error = ECVEC_NONE;

        v.memb_size = sizeof(int);
        v.nmemb = SIZE_MAX;
        size_t old_nmemb = v.nmemb;
        size_t old_cap = v.nmemb_cap;
        void *old_data = v.data;

        int dummy[] = {7};
        /* start = SIZE_MAX, want = start + 10 wraps, so want < start */
        TEST_ASSERT_EQUAL_INT(-1, hooks_raw_append(&v, dummy, 10));
        TEST_ASSERT_TRUE(ECVEC_OVERFLOW & v.error);

        TEST_ASSERT_EQUAL_size_t(old_nmemb, v.nmemb);
        TEST_ASSERT_EQUAL_size_t(old_cap, v.nmemb_cap);
        TEST_ASSERT_EQUAL_PTR(old_data, v.data);
    }

    /* ---- Subtest 6: missing append hook ---- */
    {
        v.error = ECVEC_NONE;

        v.hooks.append = NULL;
        size_t old_nmemb = v.nmemb;
        void *old_data = v.data;

        int dummy[] = {1};
        TEST_ASSERT_EQUAL_INT(-1, hooks_raw_append(&v, dummy, 1));
        TEST_ASSERT_TRUE(ECVEC_MISSING_HOOK_APPEND & v.error);

        /* no changes to contents */
        TEST_ASSERT_EQUAL_size_t(old_nmemb, v.nmemb);
        TEST_ASSERT_EQUAL_PTR(old_data, v.data);
    }

    /* cleanup underlying storage using the allocator hook */
    if (v.data) {
        g_hooks.free(v.data);
    }
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

void test_ALL_default(void) {
    RUN_TEST(test_default_cvec_grow);
    RUN_TEST(test_default_cvec_resize);
    RUN_TEST(test_default_cvec_push);
    RUN_TEST(test_default_cvec_pushn);
    RUN_TEST(test_default_cvec_vpushf);
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
    RUN_TEST(test_hooks_raw_vpushf);
    RUN_TEST(test_hooks_raw_append);
    RUN_TEST(test_hooks_raw_insert);
    RUN_TEST(test_hooks_raw_erase);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_cvec_hooks_init);
    RUN_TEST(test_cvec_init);
    test_ALL_default();
    test_ALL_hooks_raw();

    UNITY_END();

    return 0;
}