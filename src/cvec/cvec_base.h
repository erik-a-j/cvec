#ifndef CVEC_BASE_H
#define CVEC_BASE_H
#include "cvec_types.h"
#include "cvec_default_hooks.h"

/**
 * @brief initialize `cvec_t` with default values
 * @param (cvec_t*)vec pointer to `cvec_t` struct
 * @param (size_t)memb_size size of underlying type in bytes
 * @param (cvec_hooks_t)hooks `cvec_hooks_t` struct by value
 * @note see `cvec_hooks_t cvec_hooks_init(void)`
 */
int cvec_init(cvec_t *vec, size_t memb_size, cvec_hooks_t hooks);
void cvec_free(cvec_t *vec);
int cvec_resize(cvec_t *vec, size_t nmemb);
int cvec_reserve(cvec_t *vec, size_t nmemb);
int cvec_push_back(cvec_t *vec, const void *elem);
int cvec_push_back_n(cvec_t *vec, const void *src, size_t count);

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
        #if !defined(_STDLIB_H) || defined(CVEC_CUSTOM_ALLOCATORS)
          .alloc = NULL, .realloc = NULL, .free = NULL,
        #else
          .alloc = malloc, .realloc = realloc, .free = free,
        #endif
        #if !defined(_STRING_H) || defined(CVEC_CUSTOM_MEMCPY)
          .memcpy = NULL,
        #else
          .memcpy = memcpy,
        #endif
        .grow = cvec_default_grow
    };
    return h;
}
static inline size_t cvec_capacity(const cvec_t *vec) { return vec->nmemb_cap * vec->memb_size; }
static inline size_t cvec_size(const cvec_t *vec) { return vec->nmemb * vec->memb_size; }

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


#endif /*CVEC_BASE_H*/