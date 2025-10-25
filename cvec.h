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
#ifndef NDEBUG
# include <stdio.h>
# include <string.h>
#endif
#ifdef _STDIO_H
# include <stdarg.h>
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

typedef void *(*alloc_fn_t)(size_t size);
typedef void *(*realloc_fn_t)(void *ptr, size_t size);
typedef void  (*free_fn_t)(void *ptr);
typedef void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n);
typedef size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

/**
 * @brief see struct definition for info
 * @note see `cvec_hooks_t cvec_hooks_init(void)`
 */
typedef struct cvec_hooks_t {

    /** @brief _typedef_ : `void *(*alloc_fn_t)(size_t size)`
     *  @param (size_t)size size in bytes
     *  @return ___on success:___ pointer to start of allocated memory, ___on fail:___ NULL
     */
    alloc_fn_t   alloc;

    /** @brief _typedef_ : `void *(*realloc_fn_t)(void *ptr, size_t size)`
     *  @param (void*)ptr pointer to start of allocated memory
     *  @param (size_t)size size in bytes
     *  @return ___on success:___ pointer to start of reallocated memory, ___on fail:___ NULL
     */
    realloc_fn_t realloc;

    /** @brief _typedef_ : `void (*free_fn_t)(void *ptr)`
     *  @param (void*)ptr pointer to start of allocated memory
     */
    free_fn_t    free;

    /** @brief _typedef_ : `void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n)`
     *  @param (void*restrict)dst pointer to destination
     *  @param (void*restrict)src pointer to source
     *  @param (size_t)n size in bytes
     *  @return ___on success:___ pointer to start of dst, ___on fail:___ NULL
     */ 
    memcpy_fn_t  memcpy;

    /** @brief _typedef_ : `size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size)`
     *  @param (size_t)old_nmemb previous number of elements
     *  @param (size_t)new_nmemb new number of elements
     *  @param (size_t)memb_size size of one element in bytes
     *  @return ___on success:___ value above 0, ___on fail:___ 0
     */
    grow_fn_t    grow;
} cvec_hooks_t;

/**
 * @brief see struct definition for info
 */
typedef struct cvec_t {
    void   *data;       /*pointer to underlying data*/
    size_t  nmemb_cap;  /*number of members capacity*/
    size_t  nmemb;      /*number of members currently used*/
    size_t  memb_size;  /*size of one member in bytes*/
    cvec_error_t error; /*error flags, see definition of `enum cvec_error_t`*/
    cvec_hooks_t fn;    /*hooks for fine control over how instance of cvec_t is handled, see `struct cvec_hooks_t`*/
} cvec_t;

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

/**
 * @brief 
 * `cvec_*` functions requires that __All__ of the __hooks__ must be valid `pointer to function`.  
 * If none of the `CVEC_CUSTOM*` preprocessor definitions are defined
 * prior to including `"cvec.h"`, this function will set each member of returned
 * `cvec_hooks_t struct` to a default valid `pointer to function`.
 * 
 * @attention
 * All `allocator hooks` will be initialized to `NULL` if `CVEC_CUSTOM_ALLOCATORS` is defined
 * and `<stdlib.h>` is not included prior to including `"cvec.h"`.
 * @attention
 * The `memcpy hook` will be initialized to `NULL` if `CVEC_CUSTOM_MEMCPY` is defined
 * and `<string.h>` is not included prior to including `"cvec.h"`. 
 * 
 * @return `struct cvec_hooks_t`
 * @note see `struct cvec_hooks_t`
 */
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
/**
 * @brief initialize `cvec_t` with default values
 * @param (cvec_t*)vec pointer to `cvec_t` struct
 * @param (size_t)memb_size size of underlying type in bytes
 * @param (cvec_hooks_t)hooks `cvec_hooks_t` struct by value
 * @note see `cvec_hooks_t cvec_hooks_init(void)`
 */
static inline void cvec_init(cvec_t *vec, size_t memb_size, cvec_hooks_t hooks) {
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    vec->memb_size = memb_size;
    vec->error = ECVEC_NONE;
    vec->fn = hooks;
}

static inline size_t cvec_capacity(const cvec_t *vec) { return vec->nmemb_cap * vec->memb_size; }
static inline size_t cvec_size(const cvec_t *vec) { return vec->nmemb * vec->memb_size; }

/*** + Internal Helpers ***/
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
/*** - Internal Helpers ***/

static inline void cvec_free(cvec_t *vec) {
    cvec_raw_free(vec, vec->data);
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
}
static int cvec_resize(cvec_t *vec, size_t nmemb) {
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
static int cvec_reserve(cvec_t *vec, size_t nmemb) {
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

static inline void *cvec_steal(cvec_t *vec) {
    if (vec->error) return NULL;
    if (!vec->data) return NULL;
    void *stolen = vec->data;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    return stolen;
}

static inline void *cvec_at(const cvec_t *vec, size_t index) {
    return (index < vec->nmemb_cap)? (char*)vec->data + index * vec->memb_size : NULL;
}
static int cvec_push_back(cvec_t *vec, const void *elem) {
    if (cvec_reserve(vec, vec->nmemb+1) != 0)
        return -1;
    if (!cvec_raw_memcpy(vec, (char*)vec->data + vec->nmemb * vec->memb_size, elem, vec->memb_size))
        return -1;
    vec->nmemb += 1;
    return 0;
}
static int cvec_push_back_n(cvec_t *vec, const void *src, size_t count) {
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
#ifdef _STRING_H
static inline int cvec_push_back_str(cvec_t *vec, const char *str) {
    size_t len_str = strlen(str);
    return cvec_push_back_n(vec, str, len_str);
}
#endif
#ifdef _STDIO_H
static int cvec_push_back_vfmt(cvec_t *vec, const char *fmt, ...) {
    if (vec->memb_size != 1) return -1;
    va_list ap;
    va_start(ap, fmt);

    va_list ap2;
    va_copy(ap2, ap);
    int needed = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    if (needed < 0) {
        va_end(ap);
        return -1;
    }

    if (cvec_reserve(vec, vec->nmemb + (size_t)needed+1) != 0) {
        va_end(ap);
        return -1;
    }

    int written = vsnprintf((char*)vec->data + vec->nmemb * vec->memb_size, (size_t)needed+1, fmt, ap);
    va_end(ap);
    if (written < 0) return -1;

    vec->nmemb += (size_t)written;
    return 0;
}
#endif


#ifndef NDEBUG
#define CVEC_AS_PTR(vec)      \
    _Generic((vec),           \
        cvec_t: &(vec),       \
        const cvec_t: &(vec), \
        cvec_t*: (vec),       \
        const cvec_t*: (vec)  \
    )
#define cvec_push_back_strliteral(vec, strliteral) \
    cvec_push_back_n(vec, strliteral, sizeof(strliteral)-1)
#define cvec_dump(vec) \
    cvec_dump_with_name(CVEC_AS_PTR(vec), #vec)

static inline char *cvec_dump_with_name(cvec_t *vec, const char *name) {
    char *dump = NULL;
    cvec_t d;
    cvec_init(&d, sizeof(char), vec->fn);
    cvec_reserve(&d, 1024);
    
    if (cvec_push_back_str(&d, name) != 0) goto End;
    if (cvec_push_back_strliteral(&d, ": {\n") != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .memb_size: %zu,\n", vec->memb_size) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .nmemb_cap: %zu,\n", vec->nmemb_cap) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .nmemb: %zu,\n", vec->nmemb) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .error: %zu,\n", vec->error) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .fn.alloc: %p,\n", vec->fn.alloc) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .fn.realloc: %p,\n", vec->fn.realloc) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .fn.free: %p,\n", vec->fn.free) != 0) goto End;
    if (cvec_push_back_vfmt(&d, "  .fn.grow: %p,\n", vec->fn.grow) != 0) goto End;
    if (vec->memb_size == 1) {
        if (cvec_push_back_vfmt(&d, "  .data:\n  [\n%.*s", vec->nmemb, vec->data) != 0) goto End;
        if (*(char*)cvec_at(vec, vec->nmemb-1) != '\n') {
            if (cvec_push_back_strliteral(&d, "\n") != 0) goto End;
        }
        if (cvec_push_back_strliteral(&d, "  ],\n") != 0) goto End;
    }
    if (cvec_push_back_strliteral(&d, "}\0") != 0) goto End;
    
    dump = cvec_steal(&d);
End:
    cvec_free(&d);
    return dump;
}
#endif

#endif /*CVEC_H*/