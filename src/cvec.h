#ifndef CVEC_H
#define CVEC_H
#include <assert.h>
#include "cvec_types.h"
#include "cvec_hooks.h"

static inline void cvec_init(cvec_t *vec, size_t memb_size, const cvec_hooks_t *hooks) {
    assert(memb_size > 0 && "memb_size must not be zero!");
    vec->hooks = hooks ? *hooks : (cvec_hooks_t){0};
    cvec_hooks_init(&vec->hooks, CVEC_HOOKS_INIT_PARTIAL);
    vec->memb_size = memb_size;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    vec->error = ECVEC_NONE;
}
static inline void cvec_free(cvec_t *vec) {
    hooks_raw_free(vec, vec->data);
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
}
static inline int cvec_reserve(cvec_t *vec, size_t nmemb) {
    if (nmemb == 0 || nmemb <= vec->nmemb_cap) {
        return 0;
    }

    size_t new_nmemb = hooks_raw_grow(vec, vec->nmemb_cap, nmemb, vec->memb_size);
    if (new_nmemb == 0) {
        return -1;
    }
    return hooks_raw_resize(vec, new_nmemb);
}
static inline int cvec_resize(cvec_t *vec, size_t nmemb) {
    return hooks_raw_resize(vec, nmemb);
}
static inline int cvec_push(cvec_t *vec, const void *elem) {
    return hooks_raw_push(vec, elem);
}
static inline int cvec_pushn(cvec_t *vec, const void *elem, size_t count) {
    return hooks_raw_pushn(vec, elem, count);
}
static inline void *cvec_insert(cvec_t *vec, const void *elem, size_t index) {
    return hooks_raw_insert(vec, elem, index);
}
static inline void *cvec_erase(cvec_t *vec, size_t first, size_t last) {
    return hooks_raw_erase(vec, first, last);
}

static inline int cvec_shrink_to_fit(cvec_t *vec) {
    return hooks_raw_resize(vec, vec->nmemb ? vec->nmemb : 0);
}
static inline int cvec_shrink_to(cvec_t *vec, size_t nmemb) {
    if (nmemb >= vec->nmemb_cap) {
        return 0;
    }
    return hooks_raw_resize(vec, nmemb);
}

static inline void cvec_pop(cvec_t *vec) {
    if (vec->nmemb > 0) {
        vec->nmemb -= 1;
    }
}
static inline void cvec_clear(cvec_t *vec) {
    vec->nmemb = 0;
}

static inline int cvec_empty(const cvec_t *vec) {
    return vec->nmemb == 0;
}
static inline size_t cvec_capacity(const cvec_t *vec) {
    return vec->nmemb_cap;
}
static inline size_t cvec_size(const cvec_t *vec) {
    return vec->nmemb;
}
static inline size_t cvec_max_size(const cvec_t *vec) {
    return SIZE_MAX / vec->memb_size;
}

static inline void *cvec_data(cvec_t *vec);
static inline const void *cvec_cdata(const cvec_t *vec);
static inline void *cvec_front(cvec_t *vec);
static inline const void *cvec_cfront(const cvec_t *vec);
static inline void *cvec_back(cvec_t *vec);
static inline const void *cvec_cback(const cvec_t *vec);
static inline void *cvec_at(cvec_t *vec, size_t index);
static inline const void *cvec_cat(const cvec_t *vec, size_t index);
static inline void *cvec_steal(cvec_t *vec);
#define CVEC_T_BASE_
#include "cvec_new.h"

#endif /*CVEC_H*/
