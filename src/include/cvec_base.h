#ifndef CVEC_BASE_H
#define CVEC_BASE_H
#include "cvec_api.h"
#include "cvec_types.h"

/**
 * @brief initialize `cvec_t` with default values
 * @param (cvec_t*)vec pointer to `cvec_t` struct
 * @param (size_t)memb_size size of underlying type in bytes
 * @param (cvec_hooks_t)hooks `cvec_hooks_t` struct by value
 * @note see `cvec_hooks_t cvec_hooks_init(void)`
 */
CVEC_API int cvec_init(cvec_t *vec, size_t memb_size, cvec_hooks_t hooks);
CVEC_API void cvec_free(cvec_t *vec);
CVEC_API int cvec_resize(cvec_t *vec, size_t nmemb);
CVEC_API int cvec_reserve(cvec_t *vec, size_t nmemb);
CVEC_API int cvec_push_back(cvec_t *vec, const void *elem);
CVEC_API int cvec_push_back_n(cvec_t *vec, const void *src, size_t count);

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