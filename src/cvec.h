#ifndef CVEC_H
#define CVEC_H
#include "cvec_hooks.h"
#include "cvec_types.h"

void cvec_init(cvec_t *vec, size_t memb_size, const cvec_hooks_t *hooks);

void cvec_free(cvec_t *vec);

int cvec_resize(cvec_t *vec, size_t nmemb);

int cvec_reserve(cvec_t *vec, size_t nmemb);

int cvec_push(cvec_t *vec, const void *elem);

int cvec_pushn(cvec_t *vec, const void *elem, size_t count);

static inline size_t cvec_capacity(const cvec_t *vec) {
    return vec->nmemb_cap;
}

static inline size_t cvec_lenght(const cvec_t *vec) {
    return vec->nmemb;
}

static inline int cvec_shrink_to_fit(cvec_t *vec) {
    return cvec_resize(vec, vec->nmemb ? vec->nmemb : 0);
}

static inline int cvec_shrink(cvec_t *vec, size_t nmemb) {
    if (nmemb >= vec->nmemb_cap) {
        return 0;
    }
    return cvec_resize(vec, nmemb);
}

static inline void *cvec_steal(cvec_t *vec) {
    void *stolen = vec->data;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    return stolen;
}

static inline void *cvec_at(cvec_t *vec, size_t index) {
    if (index >= vec->nmemb || (vec->memb_size != 0 && index > SIZE_MAX / vec->memb_size)) {
        return NULL;
    }
    return (void *)((char *)vec->data + index * vec->memb_size);
}
static inline const void *cvec_cat(const cvec_t *vec, size_t index) {
    if (index >= vec->nmemb || (vec->memb_size != 0 && index > SIZE_MAX / vec->memb_size)) {
        return NULL;
    }
    return (const void *)((const char *)vec->data + index * vec->memb_size);
}

#endif /*CVEC_H*/