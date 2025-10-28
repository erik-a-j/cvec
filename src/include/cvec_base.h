//@file
#ifndef CVEC_BASE_H
#define CVEC_BASE_H
#include "cvec_api.h"
#include "cvec_types.h"

//cvec_init
CVEC_API void cvec_init(cvec_t *vec, size_t memb_size, const cvec_hooks_t *hooks);

//cvec_free
CVEC_API void cvec_free(cvec_t *vec);

//cvec_resize
CVEC_API int cvec_resize(cvec_t *vec, size_t nmemb);

//cvec_reserve
CVEC_API int cvec_reserve(cvec_t *vec, size_t nmemb);

//cvec_push
CVEC_API int cvec_push(cvec_t *vec, const void *elem);

//cvec_push_n
CVEC_API int cvec_push_n(cvec_t *vec, const void *src, size_t count);

//cvec_capacity
static inline size_t cvec_capacity(const cvec_t *vec) {
    return vec->nmemb_cap * vec->memb_size;
}

//cvec_size
static inline size_t cvec_size(const cvec_t *vec) {
    return vec->nmemb * vec->memb_size;
}

//cvec_shrink_to_fit
static inline int cvec_shrink_to_fit(cvec_t *vec) {
    return cvec_resize(vec, vec->nmemb ? vec->nmemb : 0);
}

//cvec_shrink
static inline int cvec_shrink(cvec_t *vec, size_t nmemb) {
    if (nmemb >= vec->nmemb_cap) { return 0; }
    return cvec_resize(vec, nmemb);
}

//cvec_steal
static inline void *cvec_steal(cvec_t *vec) {
    void *stolen = vec->data;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    return stolen;
}

//cvec_at
static inline void *cvec_at(const cvec_t *vec, size_t index) {
    return (index < vec->nmemb_cap) ? (char *)vec->data + index * vec->memb_size : NULL;
}

#endif /*CVEC_BASE_H*/