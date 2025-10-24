#ifndef CVEC_H
#define CVEC_H

#include <stddef.h>
#include <stdint.h>
#ifndef CVEC_CUSTOM_MEMCPY
# include <string.h>
#endif
#ifndef CVEC_CUSTOM_ALLOCATORS
# include <stdlib.h>
#endif

enum cvec_error_t {
    ECVEC_NONE               = 0u
   ,ECVEC_MISSING_ALLOC_FN   = (1u << 0)
   ,ECVEC_MISSING_REALLOC_FN = (1u << 1)
   ,ECVEC_MISSING_FREE_FN    = (1u << 2)
   ,ECVEC_MISSING_GROW_FN    = (1u << 3)
   ,ECVEC_MISSING_MEMCPY_FN  = (1u << 4)
   ,ECVEC_OVERFLOW           = (1u << 5)
   ,ECVEC_ALLOC              = (1u << 6)
   ,ECVEC_REALLOC            = (1u << 7)
   ,ECVEC_RESIZE             = (1u << 8)
   ,ECVEC_GROW               = (1u << 9)
};
typedef size_t cvec_error_t;

typedef void *(*alloc_fn_t)(size_t);
typedef void *(*realloc_fn_t)(void *,size_t);
typedef void  (*free_fn_t)(void *);
typedef void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n);
typedef size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

typedef struct cvec_hooks_t {
    alloc_fn_t   alloc;
    realloc_fn_t realloc;
    free_fn_t    free;
    memcpy_fn_t  memcpy;
    grow_fn_t    grow;
} cvec_hooks_t;

typedef struct cvec_t {
    void   *data;
    size_t  nmemb_cap;
    size_t  nmemb;
    size_t  memb_size;
    cvec_error_t error;
    cvec_hooks_t fn;
} cvec_t;

static inline size_t cvec_capacity(const cvec_t *vec) { return vec->nmemb_cap * vec->memb_size; }
static inline size_t cvec_size(const cvec_t *vec) { return vec->nmemb * vec->memb_size; }

#define CVEC_GROW_ERROR 0
#define CVEC_NMEMB_MAX(memb_size) ((memb_size) ? (SIZE_MAX / (memb_size)) : 0)
static size_t cvec_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (memb_size == 0)
        return CVEC_GROW_ERROR;
    if (old_nmemb >= new_nmemb)
        return (old_nmemb <= CVEC_NMEMB_MAX(memb_size))? old_nmemb : CVEC_GROW_ERROR;

    size_t n = old_nmemb ? old_nmemb + old_nmemb/(size_t)2 + (size_t)1 : (size_t)8;
    if (n < old_nmemb) n = CVEC_NMEMB_MAX(memb_size);
    if (n < new_nmemb) n = new_nmemb;

    if (n > CVEC_NMEMB_MAX(memb_size))
        return CVEC_GROW_ERROR;
    return n;
}
static inline void *cvec_raw_realloc(cvec_t *vec, size_t size) {
    if (!vec->fn.realloc) { vec->error |= ECVEC_MISSING_REALLOC_FN; return NULL; }
    return vec->fn.realloc(vec->data, size);
}
static inline void *cvec_raw_alloc(cvec_t *vec, size_t size) {
    if (!vec->fn.alloc) { vec->error |= ECVEC_MISSING_ALLOC_FN; return NULL; }
    return vec->fn.alloc(size);
}
static inline void cvec_raw_free(cvec_t *vec, void *ptr) {
    if (!vec->fn.free) { vec->error |= ECVEC_MISSING_FREE_FN; return; }
    vec->fn.free(ptr);
}
static inline void *cvec_raw_memcpy(cvec_t *vec, void *restrict dst, const void *restrict src, size_t n) {
    if (!vec->fn.memcpy) { vec->error |= ECVEC_MISSING_MEMCPY_FN; return NULL; }
    return vec->fn.memcpy(dst, src, n);
}

static inline void cvec_free(cvec_t *vec) {
    cvec_raw_free(vec, vec->data);
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
}

static inline int cvec_resize(cvec_t *vec, size_t nmemb) {
    if (nmemb == vec->nmemb_cap) return 0;
    if (nmemb == 0) {
        if (vec->data) cvec_raw_free(vec, vec->data);
        vec->data = NULL;
        vec->nmemb_cap = vec->nmemb = 0;
        return 0;
    }
    if (nmemb > CVEC_NMEMB_MAX(vec->memb_size)) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    void *newdata = vec->nmemb_cap ? cvec_raw_realloc(vec, nmemb * vec->memb_size)
                                   : cvec_raw_alloc(vec, nmemb * vec->memb_size);
    if (!newdata) {
        vec->error |= ECVEC_RESIZE;
        return -1;
    }
    vec->data = newdata;
    vec->nmemb_cap = nmemb;
    if (vec->nmemb > vec->nmemb_cap)
        vec->nmemb = vec->nmemb_cap;
    return 0;
}
static inline int cvec_reserve(cvec_t *vec, size_t nmemb) {
    if (nmemb == 0 || nmemb <= vec->nmemb_cap) return 0;
    
    if (!vec->fn.grow) {
        vec->error |= ECVEC_MISSING_GROW_FN;
        return -1;
    }
    size_t new_nmemb = vec->fn.grow(vec->nmemb_cap, nmemb, vec->memb_size);
    if (new_nmemb == CVEC_GROW_ERROR) {
        vec->error |= ECVEC_GROW;
        return -1;
    }
    return cvec_resize(vec, new_nmemb);
}
static inline int cvec_shrink_to_fit(cvec_t *vec) {
    return cvec_resize(vec, vec->nmemb ? vec->nmemb : 0);
}
static inline int cvec_shrink(cvec_t *vec, size_t nmemb) {
    if (nmemb >= vec->nmemb_cap) return 0;
    return cvec_resize(vec, nmemb);
}

static inline void cvec_init(cvec_t *vec, size_t memb_size, cvec_hooks_t hooks) {
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    vec->memb_size = memb_size;
    vec->error = ECVEC_NONE;
    vec->fn = hooks;
}

static inline cvec_hooks_t cvec_hooks_init(void) {
    cvec_hooks_t h = {
        #if defined(CVEC_CUSTOM_ALLOCATORS) && !defined(_STDLIB_H)
          .alloc = NULL, .realloc = NULL, .free = NULL,
        #else
          .alloc = malloc, .realloc = realloc, .free = free,
        #endif
        #if defined(CVEC_CUSTOM_MEMCPY) && !defined(_STRING_H)
          .memcpy = NULL,
        #else
          .memcpy = memcpy,
        #endif
        .grow = cvec_grow
    };
    return h;
}

static inline void *cvec_at(const cvec_t *vec, size_t index) {
    return (index < vec->nmemb_cap)? (char*)vec->data + index * vec->memb_size : NULL;
}

static inline int cvec_push_back(cvec_t *vec, const void *elem) {
    if (cvec_reserve(vec, vec->nmemb+1) != 0)
        return -1;
    if (!cvec_raw_memcpy(vec, (char*)vec->data + vec->nmemb * vec->memb_size, elem, vec->memb_size))
        return -1;
    vec->nmemb += 1;
    return 0;
}
static inline int cvec_push_back_n(cvec_t *vec, const void *src, size_t count) {
    if (count == 0) return 0;
    size_t want = vec->nmemb + count;
    if (want < vec->nmemb) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    if (cvec_reserve(vec, want) != 0)
        return -1;
    if (!cvec_raw_memcpy(vec, (char*)vec->data + vec->nmemb * vec->memb_size, src, count * vec->memb_size))
        return -1;
    vec->nmemb = want;
    return 0;
}

#endif /*CVEC_H*/