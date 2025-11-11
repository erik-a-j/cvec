#include <stdint.h>
#include "cvec.h"

size_t default_cvec_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (memb_size == 0) {
        return 0;
    }
    if (old_nmemb >= new_nmemb) {
        return (old_nmemb <= SIZE_MAX / memb_size) ? old_nmemb : 0;
    }

    size_t n = old_nmemb ? old_nmemb + old_nmemb / (size_t)2 + (size_t)1 : (size_t)8;
    if (n < old_nmemb) {
        n = SIZE_MAX / memb_size;
    }
    if (n < new_nmemb) {
        n = new_nmemb;
    }

    if (n > SIZE_MAX / memb_size) {
        return 0;
    }
    return n;
}

int default_cvec_resize(cvec_t *vec, size_t nmemb) {
    if (nmemb == vec->nmemb_cap) {
        return 0;
    }
    if (nmemb == 0) {
        cvec_free(vec);
        return 0;
    }
    if (nmemb > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    void *newdata =
        vec->nmemb_cap ? hooks_raw_realloc(vec, nmemb * vec->memb_size) : hooks_raw_alloc(vec, nmemb * vec->memb_size);
    if (!newdata) {
        return -1;
    }
    vec->data = newdata;
    vec->nmemb_cap = nmemb;
    if (vec->nmemb > vec->nmemb_cap) {
        vec->nmemb = vec->nmemb_cap;
    }
    return 0;
}

int default_cvec_push(cvec_t *vec, const void *elem) {
    if (vec->nmemb == SIZE_MAX) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    if (cvec_reserve(vec, vec->nmemb + 1) != 0) {
        return -1;
    }
    if (!hooks_raw_memcpy(vec, (char *)vec->data + vec->nmemb * vec->memb_size, elem, vec->memb_size)) {
        return -1;
    }
    vec->nmemb += 1;
    return 0;
}
int default_cvec_pushn(cvec_t *vec, const void *elem, size_t count) {
    if (count == 0) {
        return 0;
    }
    size_t start = vec->nmemb;
    size_t want = start + count;
    if (want < start || count > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return -1;
    }
    if (cvec_reserve(vec, want) != 0) {
        return -1;
    }

    void *dst0 = (char *)vec->data + start * vec->memb_size;
    if (!hooks_raw_memcpy(vec, dst0, elem, vec->memb_size)) {
        return -1;
    }

    size_t written = 1;
    while (written < count) {
        size_t to_copy = (written <= count - written) ? written : (count - written);
        void *dst = (char *)dst0 + written * vec->memb_size;
        if (!hooks_raw_memcpy(vec, dst, dst0, to_copy * vec->memb_size)) {
            return -1;
        }
        written += to_copy;
    }
    vec->nmemb = want;

    return 0;
}

void *default_cvec_insert(cvec_t *vec, const void *elem, size_t index) {
    if (index > vec->nmemb) {
        vec->error |= ECVEC_INVALID_INDEX;
        return NULL;
    }
    if (vec->memb_size == 0 || vec->nmemb == SIZE_MAX || index > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return NULL;
    }
    if (cvec_reserve(vec, vec->nmemb + 1) != 0) {
        return NULL;
    }
    char *base = (char *)vec->data;
    char *data = base + index * vec->memb_size;

    size_t tail_elems = vec->nmemb - index;
    if (tail_elems > 0) {
        if (tail_elems > SIZE_MAX / vec->memb_size) {
            vec->error |= ECVEC_OVERFLOW;
            return NULL;
        }
        size_t bytes = tail_elems * vec->memb_size;

        if (!hooks_raw_memmove(vec, data + vec->memb_size, data, bytes)) {
            return NULL;
        }
    }

    if (!hooks_raw_memcpy(vec, data, elem, vec->memb_size)) {
        return NULL;
    }

    vec->nmemb += 1;
    return data;
}

void *default_cvec_erase(cvec_t *vec, size_t first, size_t last) {
    if (vec->nmemb == 0) {
        vec->error |= ECVEC_INVALID_INDEX;
        return NULL;
    }
    if (first > last) {
        size_t tmp = first;
        first = last;
        last = tmp;
    }
    if (first >= vec->nmemb) {
        vec->error |= ECVEC_INVALID_INDEX;
        return NULL;
    }
    if (last >= vec->nmemb) {
        last = vec->nmemb - 1;
    }

    size_t count = last - first + 1;
    size_t tail_elems = vec->nmemb - (last + 1);

    if (vec->memb_size == 0) {
        vec->error |= ECVEC_OVERFLOW;
        return NULL;
    }
    if (first > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return NULL;
    }
    if (last > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return NULL;
    }
    if (tail_elems > 0 && tail_elems > SIZE_MAX / vec->memb_size) {
        vec->error |= ECVEC_OVERFLOW;
        return NULL;
    }

    char *base = (char *)vec->data;
    char *dst_first = base + first * vec->memb_size;
    char *src_after = base + (last + 1) * vec->memb_size;
    if (tail_elems > 0) {
        size_t bytes = tail_elems * vec->memb_size;
        if (!hooks_raw_memmove(vec, dst_first, src_after, bytes)) {
            return NULL;
        }
    }
    vec->nmemb -= count;
    return (tail_elems > 0) ? (void *)dst_first : NULL;
}