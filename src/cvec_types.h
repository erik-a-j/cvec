#ifndef CVEC_TYPES_H
#define CVEC_TYPES_H
#include <stddef.h>
#include <stdint.h>

#if __STDC_VERSION__ >= 202311L
#define CVEC_ERROR_T cvec_error_t : uint_least32_t
#else
#define CVEC_ERROR_T cvec_error_t
#endif
enum CVEC_ERROR_T {
    ECVEC_NONE = 0u,
    ECVEC_MISSING_ALLOC_FN = (1u << 0),
    ECVEC_MISSING_REALLOC_FN = (1u << 1),
    ECVEC_MISSING_FREE_FN = (1u << 2),
    ECVEC_MISSING_GROW_FN = (1u << 3),
    ECVEC_MISSING_MEMCPY_FN = (1u << 4),
    ECVEC_OVERFLOW = (1u << 5)
};
#if __STDC_VERSION__ >= 202311L
typedef enum cvec_error_t cvec_error_t;
#else
typedef uint_least32_t cvec_error_t;
#endif

typedef void *(*alloc_fn_t)(size_t size);
typedef void *(*realloc_fn_t)(void *ptr, size_t size);
typedef void (*free_fn_t)(void *ptr);
typedef void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n);
typedef size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

typedef struct cvec_hooks_t {
    alloc_fn_t alloc;
    realloc_fn_t realloc;
    free_fn_t free;
    memcpy_fn_t memcpy;
    grow_fn_t grow;
} cvec_hooks_t;

typedef struct cvec_t {
    void *data;
    size_t nmemb_cap;
    size_t nmemb;
    size_t memb_size;
    cvec_error_t error;
    cvec_hooks_t hooks;
} cvec_t;

#endif /*CVEC_TYPES_H*/