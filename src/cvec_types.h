#ifndef CVEC_TYPES_H
#define CVEC_TYPES_H
#include <stddef.h>
#include <stdint.h>

#define ECVEC_NONE                 0u
#define ECVEC_OVERFLOW             (1u << 0)
#define ECVEC_INVALID_INDEX        (1u << 1)
#define ECVEC_MISSING_HOOK_ALLOC   (1u << 2)
#define ECVEC_MISSING_HOOK_REALLOC (1u << 3)
#define ECVEC_MISSING_HOOK_FREE    (1u << 4)
#define ECVEC_MISSING_HOOK_MEMCPY  (1u << 5)
#define ECVEC_MISSING_HOOK_MEMMOVE (1u << 6)
#define ECVEC_MISSING_HOOK_GROW    (1u << 7)
#define ECVEC_MISSING_HOOK_RESIZE  (1u << 8)
#define ECVEC_MISSING_HOOK_PUSH    (1u << 9)
#define ECVEC_MISSING_HOOK_PUSHN   (1u << 10)
#define ECVEC_MISSING_HOOK_INSERT  (1u << 11)
#define ECVEC_MISSING_HOOK_ERASE   (1u << 12)
typedef uint_least32_t cvec_error_t;

typedef struct cvec_t cvec_t;
typedef void *(*alloc_fn_t)(size_t size);
typedef void *(*realloc_fn_t)(void *ptr, size_t size);
typedef void (*free_fn_t)(void *ptr);
typedef void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n);
typedef void *(*memmove_fn_t)(void *dst, const void *src, size_t n);
typedef size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size);
typedef int (*resize_fn_t)(cvec_t *vec, size_t nmemb);
typedef int (*push_fn_t)(cvec_t *vec, const void *elem);
typedef int (*pushn_fn_t)(cvec_t *vec, const void *elem, size_t count);
typedef void *(*insert_fn_t)(cvec_t *vec, const void *elem, size_t index);
typedef void *(*erase_fn_t)(cvec_t *vec, size_t first, size_t last);

typedef struct cvec_hooks_t {
    alloc_fn_t alloc;
    realloc_fn_t realloc;
    free_fn_t free;
    memcpy_fn_t memcpy;
    memmove_fn_t memmove;
    grow_fn_t grow;
    resize_fn_t resize;
    push_fn_t push;
    pushn_fn_t pushn;
    insert_fn_t insert;
    erase_fn_t erase;
} cvec_hooks_t;

struct cvec_t {
    void *data;
    size_t nmemb_cap;
    size_t nmemb;
    size_t memb_size;
    cvec_error_t error;
    cvec_hooks_t hooks;
};

#endif /*CVEC_TYPES_H*/
