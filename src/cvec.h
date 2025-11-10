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

void *cvec_insert(cvec_t *vec, const void *elem, size_t index);

void *cvec_erase(cvec_t *vec, size_t first, size_t last);

static inline int cvec_shrink_to_fit(cvec_t *vec) {
    return cvec_resize(vec, vec->nmemb ? vec->nmemb : 0);
}

static inline int cvec_shrink_to(cvec_t *vec, size_t nmemb) {
    if (nmemb >= vec->nmemb_cap) {
        return 0;
    }
    return cvec_resize(vec, nmemb);
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

static inline void cvec_clear(cvec_t *vec) {
    vec->nmemb = 0;
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

static inline void cvec_pop(cvec_t *vec) {
    if (vec->nmemb > 0) {
        vec->nmemb -= 1;
    }
}

#endif /*CVEC_H*/
