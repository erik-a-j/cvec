#include <stdint.h>
#include "cvec_impl.h"

size_t cvec_default_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (memb_size == 0) { return 0; }
    if (old_nmemb >= new_nmemb) { return (old_nmemb <= SIZE_MAX / memb_size) ? old_nmemb : 0; }

    size_t n = old_nmemb ? old_nmemb + old_nmemb / (size_t)2 + (size_t)1 : (size_t)8;
    if (n < old_nmemb) { n = SIZE_MAX / memb_size; }
    if (n < new_nmemb) { n = new_nmemb; }

    if (n > SIZE_MAX / memb_size) { return 0; }
    return n;
}

static void *cvec_raw_realloc(cvec_t *vec, size_t size) {
    if (!vec->hooks.realloc) {
        vec->error |= ECVEC_MISSING_REALLOC_FN;
        return NULL;
    }
    return vec->hooks.realloc(vec->data, size);
}

static void *cvec_raw_alloc(cvec_t *vec, size_t size) {
    if (!vec->hooks.alloc) {
        vec->error |= ECVEC_MISSING_ALLOC_FN;
        return NULL;
    }
    return vec->hooks.alloc(size);
}

static void cvec_raw_free(cvec_t *vec, void *ptr) {
    if (!vec->hooks.free) {
        vec->error |= ECVEC_MISSING_FREE_FN;
        return;
    }
    vec->hooks.free(ptr);
}

static void *cvec_raw_memcpy(cvec_t *vec, void *restrict dst, const void *restrict src, size_t n) {
    if (!vec->hooks.memcpy) {
        vec->error |= ECVEC_MISSING_MEMCPY_FN;
        return NULL;
    }
    return vec->hooks.memcpy(dst, src, n);
}

static size_t cvec_raw_grow(cvec_t *vec, size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (!vec->hooks.grow) {
        vec->error |= ECVEC_MISSING_GROW_FN;
        return 0;
    }
    return vec->hooks.grow(old_nmemb, new_nmemb, memb_size);
}

/*static void cvec_ensure_hooks(cvec_hooks_t *h) {
    cvec_hooks_t dh = cvec_hooks_init();
    if (!h->alloc) {
        h->alloc = dh.alloc;
    }
    if (!h->realloc) {
        h->realloc = dh.realloc;
    }
    if (!h->free) {
        h->free = dh.free;
    }
    if (!h->memcpy) {
        h->memcpy = dh.memcpy;
    }
    if (!h->grow) {
        h->grow = dh.grow;
    }
}*/

void cvec_init(cvec_t *vec, size_t memb_size, const cvec_hooks_t *hooks) {
    vec->hooks = hooks ? *hooks : (cvec_hooks_t){0};
    cvec_hooks_init(&vec->hooks, CVEC_HOOKS_INIT_PARTIAL);

    vec->memb_size = memb_size;
    vec->data      = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    vec->error                  = ECVEC_NONE;
}

void cvec_free(cvec_t *vec) {
    cvec_raw_free(vec, vec->data);
    if (!(vec->error & ECVEC_MISSING_FREE_FN)) {
        vec->error     = ECVEC_NONE;
        vec->data      = NULL;
        vec->nmemb_cap = vec->nmemb = 0;
    }
}

int cvec_resize(cvec_t *vec, size_t nmemb) {
    if (nmemb == vec->nmemb_cap) { return 0; }
    if (nmemb == 0) {
        if (vec->data) { cvec_raw_free(vec, vec->data); }
        vec->data      = NULL;
        vec->nmemb_cap = vec->nmemb = 0;
        return 0;
    }
    if (nmemb > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    void *newdata =
        vec->nmemb_cap ? cvec_raw_realloc(vec, nmemb * vec->memb_size) : cvec_raw_alloc(vec, nmemb * vec->memb_size);
    if (!newdata) { return -1; }
    vec->data      = newdata;
    vec->nmemb_cap = nmemb;
    if (vec->nmemb > vec->nmemb_cap) { vec->nmemb = vec->nmemb_cap; }
    return 0;
}

int cvec_reserve(cvec_t *vec, size_t nmemb) {
    if (nmemb == 0 || nmemb <= vec->nmemb_cap) { return 0; }

    size_t new_nmemb = cvec_raw_grow(vec, vec->nmemb_cap, nmemb, vec->memb_size);
    if (new_nmemb == 0) { return -1; }
    return cvec_resize(vec, new_nmemb);
}

int cvec_push(cvec_t *vec, const void *elem) {
    if (cvec_reserve(vec, vec->nmemb + 1) != 0) { return -1; }
    if (!cvec_raw_memcpy(vec, (char *)vec->data + vec->nmemb * vec->memb_size, elem, vec->memb_size)) { return -1; }
    vec->nmemb += 1;
    return 0;
}

int cvec_push_n(cvec_t *vec, const void *src, size_t count) {
    if (count == 0) { return 0; }
    size_t want = vec->nmemb + count;
    if (want < vec->nmemb) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    if (cvec_reserve(vec, want) != 0) { return -1; }
    if (!cvec_raw_memcpy(vec, (char *)vec->data + vec->nmemb * vec->memb_size, src, count * vec->memb_size)) {
        return -1;
    }
    vec->nmemb = want;
    return 0;
}
