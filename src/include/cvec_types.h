#ifndef CVEC_TYPES_H
#define CVEC_TYPES_H
#include <stddef.h>

enum cvec_error_t {
    ECVEC_NONE               = 0u,
    ECVEC_MISSING_ALLOC_FN   = (1u << 0),
    ECVEC_MISSING_REALLOC_FN = (1u << 1),
    ECVEC_MISSING_FREE_FN    = (1u << 2),
    ECVEC_MISSING_GROW_FN    = (1u << 3),
    ECVEC_MISSING_MEMCPY_FN  = (1u << 4),
    ECVEC_OVERFLOW           = (1u << 5)
};
typedef size_t cvec_error_t;

//alloc_fn_t
typedef void *(*alloc_fn_t)(size_t size);

//realloc_fn_t
typedef void *(*realloc_fn_t)(void *ptr, size_t size);

//free_fn_t
typedef void (*free_fn_t)(void *ptr);

//memcpy_fn_t
typedef void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n);

//grow_fn_t
typedef size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

//cvec_hooks_t
typedef struct cvec_hooks_t {
    alloc_fn_t alloc;
    realloc_fn_t realloc;
    free_fn_t free;
    memcpy_fn_t memcpy;
    grow_fn_t grow;
} cvec_hooks_t;

//cvec_t
typedef struct cvec_t {
    void *data;         /**< pointer to underlying data (or NULL) */
    size_t nmemb_cap;   /**< capacity in elements */
    size_t nmemb;       /**< number of elements currently used */
    size_t memb_size;   /**< size of one element in bytes (immutable after init) */
    cvec_error_t error; /**< last error flags; see `enum cvec_error_t` */
    cvec_hooks_t hooks; /**< per-instance hooks; see `struct cvec_hooks_t` */
} cvec_t;

#endif /*CVEC_TYPES_H*/