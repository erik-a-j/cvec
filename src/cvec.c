#include "cvec_impl.h"
#include <stdint.h>

#define CVEC_NMEMB_MAX(memb_size) ((memb_size) ? (SIZE_MAX / (memb_size)) : 0)
size_t cvec_default_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (memb_size == 0)
        return 0;
    if (old_nmemb >= new_nmemb)
        return (old_nmemb <= CVEC_NMEMB_MAX(memb_size))? old_nmemb : 0;

    size_t n = old_nmemb ? old_nmemb + old_nmemb/(size_t)2 + (size_t)1 : (size_t)8;
    if (n < old_nmemb) n = CVEC_NMEMB_MAX(memb_size);
    if (n < new_nmemb) n = new_nmemb;

    if (n > CVEC_NMEMB_MAX(memb_size))
        return 0;
    return n;
}
cvec_hooks_t *cvec_hooks_get(cvec_t *vec) { return &vec->_->hooks; }

static void *cvec_raw_realloc(cvec_t *vec, size_t size) {
    if (!vec->_->hooks.realloc) { vec->_->error |= ECVEC_MISSING_REALLOC_FN; return NULL; }
    return vec->_->hooks.realloc(vec->data, size);
}
static void *cvec_raw_alloc(cvec_t *vec, size_t size) {
    if (!vec->_->hooks.alloc) { vec->_->error |= ECVEC_MISSING_ALLOC_FN; return NULL; }
    return vec->_->hooks.alloc(size);
}
static void cvec_raw_free(cvec_t *vec, void *ptr) {
    if (!vec->_->hooks.free) { vec->_->error |= ECVEC_MISSING_FREE_FN; return; }
    vec->_->hooks.free(ptr);
}
static void *cvec_raw_memcpy(cvec_t *vec, void *restrict dst, const void *restrict src, size_t n) {
    if (!vec->_->hooks.memcpy) { vec->_->error |= ECVEC_MISSING_MEMCPY_FN; return NULL; }
    return vec->_->hooks.memcpy(dst, src, n);
}
static int cvec_assure_hooks(const cvec_hooks_t *h) {
    if (!h->alloc)   return 1;
    if (!h->realloc) return 1;
    if (!h->free)    return 1;
    if (!h->memcpy)  return 1;
    if (!h->grow)    return 1;
    return 0;
}

int cvec_init(cvec_t *vec, size_t memb_size, cvec_hooks_t hooks) {
    if (cvec_assure_hooks(&hooks) != 0)
        return -1;
    vec->memb_size = memb_size;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    vec->_ = NULL;
    if (!hooks.alloc) return -1;
    vec->_ = hooks.alloc(sizeof *vec->_);
    if (!vec->_) return -1;
    *vec->_ = (struct cvec_internal_t){
        .hooks = hooks,
        .error = ECVEC_NONE
    };
    return 0;
}

void cvec_free(cvec_t *vec) {
    cvec_raw_free(vec, vec->data);
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = vec->memb_size = 0;
    cvec_raw_free(vec, vec->_);
    vec->_ = NULL;
}

int cvec_resize(cvec_t *vec, size_t nmemb) {
    if (nmemb == vec->nmemb_cap) return 0;
    if (nmemb == 0) {
        if (vec->data) cvec_raw_free(vec, vec->data);
        vec->data = NULL;
        vec->nmemb_cap = vec->nmemb = 0;
        return 0;
    }
    if (nmemb > CVEC_NMEMB_MAX(vec->memb_size)) {
        vec->_->error |= ECVEC_OVERFLOW;
        return -1;
    }
    void *newdata = vec->nmemb_cap ? cvec_raw_realloc(vec, nmemb * vec->memb_size)
                                   : cvec_raw_alloc(vec, nmemb * vec->memb_size);
    if (!newdata) {
        vec->_->error |= ECVEC_RESIZE;
        return -1;
    }
    vec->data = newdata;
    vec->nmemb_cap = nmemb;
    if (vec->nmemb > vec->nmemb_cap)
        vec->nmemb = vec->nmemb_cap;
    return 0;
}
int cvec_reserve(cvec_t *vec, size_t nmemb) {
    if (nmemb == 0 || nmemb <= vec->nmemb_cap) return 0;
    
    if (!vec->_->hooks.grow) {
        vec->_->error |= ECVEC_MISSING_GROW_FN;
        return -1;
    }
    size_t new_nmemb = vec->_->hooks.grow(vec->nmemb_cap, nmemb, vec->memb_size);
    if (new_nmemb == 0) {
        vec->_->error |= ECVEC_GROW;
        return -1;
    }
    return cvec_resize(vec, new_nmemb);
}

int cvec_push_back(cvec_t *vec, const void *elem) {
    if (cvec_reserve(vec, vec->nmemb+1) != 0)
        return -1;
    if (!cvec_raw_memcpy(vec, (char*)vec->data + vec->nmemb * vec->memb_size, elem, vec->memb_size))
        return -1;
    vec->nmemb += 1;
    return 0;
}
int cvec_push_back_n(cvec_t *vec, const void *src, size_t count) {
    if (count == 0) return 0;
    size_t want = vec->nmemb + count;
    if (want < vec->nmemb) {
        vec->_->error |= ECVEC_OVERFLOW;
        return -1;
    }
    if (cvec_reserve(vec, want) != 0)
        return -1;
    if (!cvec_raw_memcpy(vec, (char*)vec->data + vec->nmemb * vec->memb_size, src, count * vec->memb_size))
        return -1;
    vec->nmemb = want;
    return 0;
}
