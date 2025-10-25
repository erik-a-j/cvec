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
# include <string.h>
#endif


#define CVEC_NMEMB_MAX(memb_size) ((memb_size) ? (SIZE_MAX / (memb_size)) : 0)
size_t cvec_default_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

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

typedef struct cvec_internal_t *cvec_internal;
/**
 * @brief see struct definition for info
 */
typedef struct cvec_t {
    void   *data;       /*pointer to underlying data*/
    size_t  nmemb_cap;  /*number of members capacity*/
    size_t  nmemb;      /*number of members currently used*/
    size_t  memb_size;  /*size of one member in bytes*/
    //cvec_error_t error; /*error flags, see definition of `enum cvec_error_t`*/
    //cvec_hooks_t fn;    /*hooks for fine control over how instance of cvec_t is handled, see `struct cvec_hooks_t`*/
    cvec_internal _;
} cvec_t;

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
        .grow = cvec_default_grow
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
int cvec_init(cvec_t *vec, size_t memb_size, cvec_hooks_t hooks);

static inline size_t cvec_capacity(const cvec_t *vec) { return vec->nmemb_cap * vec->memb_size; }
static inline size_t cvec_size(const cvec_t *vec) { return vec->nmemb * vec->memb_size; }

void cvec_free(cvec_t *vec);
int cvec_resize(cvec_t *vec, size_t nmemb);
int cvec_reserve(cvec_t *vec, size_t nmemb);

static inline int cvec_shrink_to_fit(cvec_t *vec) {
    return cvec_resize(vec, vec->nmemb ? vec->nmemb : 0);
}
static inline int cvec_shrink(cvec_t *vec, size_t nmemb) {
    if (nmemb >= vec->nmemb_cap) return 0;
    return cvec_resize(vec, nmemb);
}

static inline void *cvec_steal(cvec_t *vec) {
    if (!vec->data) return NULL;
    void *stolen = vec->data;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    return stolen;
}

static inline void *cvec_at(const cvec_t *vec, size_t index) {
    return (index < vec->nmemb_cap)? (char*)vec->data + index * vec->memb_size : NULL;
}

int cvec_push_back(cvec_t *vec, const void *elem);
int cvec_push_back_n(cvec_t *vec, const void *src, size_t count);

#define cvec_push_back_strliteral(vec, strliteral) \
    cvec_push_back_n(vec, strliteral, sizeof(strliteral)-1)
#ifdef _STRING_H
static inline int cvec_push_back_str(cvec_t *vec, const char *str) {
    size_t len_str = strlen(str);
    return cvec_push_back_n(vec, str, len_str);
}
#endif

#ifdef USE_PUSH_BACK_VFMT
int cvec_push_back_vfmt(cvec_t *vec, const char *fmt, ...);
#define CVEC_AS_PTR(vec)      \
    _Generic((vec),           \
        cvec_t: &(vec),       \
        cvec_t*: (vec),       \
        const cvec_t*: (vec)  \
    )
#define cvec_dump(vec) \
    cvec_dump_with_name(CVEC_AS_PTR(vec), #vec)
char *cvec_dump_with_name(cvec_t *vec, const char *name);
#endif /*USE_PUSH_BACK_VFMT*/

#endif /*CVEC_H*/