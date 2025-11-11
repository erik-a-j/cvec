#ifndef CVEC_HOOKS_H
#define CVEC_HOOKS_H
#include "cvec_types.h"

size_t default_cvec_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size);
int default_cvec_resize(cvec_t *vec, size_t nmemb);
int default_cvec_push(cvec_t *vec, const void *elem);
int default_cvec_pushn(cvec_t *vec, const void *elem, size_t count);
void *default_cvec_insert(cvec_t *vec, const void *elem, size_t index);
void *default_cvec_erase(cvec_t *vec, size_t first, size_t last);

#ifndef CVEC_CUSTOM_ALLOCATORS
#include <stdlib.h>
#endif
#if !defined(CVEC_CUSTOM_MEMCPY) || !defined(CVEC_CUSTOM_MEMMOVE)
#include <string.h>
#endif

#define CVEC_HOOKS_INIT_OVERWRITE 0
#define CVEC_HOOKS_INIT_PARTIAL   1

static inline void cvec_hooks_init(cvec_hooks_t *hooks, int flag) {
    cvec_hooks_t default_hooks = {
#ifdef CVEC_CUSTOM_ALLOCATORS
        .alloc = NULL,
        .realloc = NULL,
        .free = NULL,
#else
        .alloc = malloc,
        .realloc = realloc,
        .free = free,
#endif
#ifdef CVEC_CUSTOM_MEMCPY
        .memcpy = NULL,
#else
        .memcpy = memcpy,
#endif
#ifdef CVEC_CUSTOM_MEMMOVE
        .memmove = NULL,
#else
        .memmove = memmove,
#endif
        .grow = default_cvec_grow,
        .resize = default_cvec_resize,
        .push = default_cvec_push,
        .pushn = default_cvec_pushn,
        .insert = default_cvec_insert,
        .erase = default_cvec_erase,
    };

    if (flag == CVEC_HOOKS_INIT_OVERWRITE) {
        *hooks = default_hooks;
    } else if (flag == CVEC_HOOKS_INIT_PARTIAL) {
        if (!hooks->alloc) {
            hooks->alloc = default_hooks.alloc;
        }
        if (!hooks->realloc) {
            hooks->realloc = default_hooks.realloc;
        }
        if (!hooks->free) {
            hooks->free = default_hooks.free;
        }
        if (!hooks->memcpy) {
            hooks->memcpy = default_hooks.memcpy;
        }
        if (!hooks->memmove) {
            hooks->memmove = default_hooks.memmove;
        }
        if (!hooks->grow) {
            hooks->grow = default_hooks.grow;
        }
        if (!hooks->resize) {
            hooks->resize = default_hooks.resize;
        }
        if (!hooks->push) {
            hooks->push = default_hooks.push;
        }
        if (!hooks->pushn) {
            hooks->pushn = default_hooks.pushn;
        }
        if (!hooks->insert) {
            hooks->insert = default_hooks.insert;
        }
        if (!hooks->erase) {
            hooks->erase = default_hooks.erase;
        }
    }
}

static inline void *hooks_raw_alloc(cvec_t *vec, size_t size) {
    if (!vec->hooks.alloc) {
        vec->error |= ECVEC_MISSING_HOOK_ALLOC;
        return NULL;
    }
    return vec->hooks.alloc(size);
}
static inline void *hooks_raw_realloc(cvec_t *vec, size_t size) {
    if (!vec->hooks.realloc) {
        vec->error |= ECVEC_MISSING_HOOK_REALLOC;
        return NULL;
    }
    return vec->hooks.realloc(vec->data, size);
}
static inline void hooks_raw_free(cvec_t *vec, void *ptr) {
    if (!vec->hooks.free) {
        vec->error |= ECVEC_MISSING_HOOK_FREE;
        return;
    }
    vec->hooks.free(ptr);
}
static inline void *hooks_raw_memcpy(cvec_t *vec, void *restrict dst, const void *restrict src, size_t n) {
    if (!vec->hooks.memcpy) {
        vec->error |= ECVEC_MISSING_HOOK_MEMCPY;
        return NULL;
    }
    return vec->hooks.memcpy(dst, src, n);
}
static inline void *hooks_raw_memmove(cvec_t *vec, void *dst, const void *src, size_t n) {
    if (!vec->hooks.memmove) {
        vec->error |= ECVEC_MISSING_HOOK_MEMMOVE;
        return NULL;
    }
    return vec->hooks.memmove(dst, src, n);
}
static inline size_t hooks_raw_grow(cvec_t *vec, size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (!vec->hooks.grow) {
        vec->error |= ECVEC_MISSING_HOOK_GROW;
        return 0;
    }
    return vec->hooks.grow(old_nmemb, new_nmemb, memb_size);
}
static inline int hooks_raw_resize(cvec_t *vec, size_t nmemb) {
    if (!vec->hooks.resize) {
        vec->error |= ECVEC_MISSING_HOOK_RESIZE;
        return -1;
    }
    return vec->hooks.resize(vec, nmemb);
}
static inline int hooks_raw_push(cvec_t *vec, const void *elem) {
    if (!vec->hooks.push) {
        vec->error |= ECVEC_MISSING_HOOK_PUSH;
        return -1;
    }
    return vec->hooks.push(vec, elem);
}
static inline int hooks_raw_pushn(cvec_t *vec, const void *elem, size_t count) {
    if (!vec->hooks.pushn) {
        vec->error |= ECVEC_MISSING_HOOK_PUSHN;
        return -1;
    }
    return vec->hooks.pushn(vec, elem, count);
}
static inline void *hooks_raw_insert(cvec_t *vec, const void *elem, size_t index) {
    if (!vec->hooks.insert) {
        vec->error |= ECVEC_MISSING_HOOK_INSERT;
        return NULL;
    }
    return vec->hooks.insert(vec, elem, index);
}
static inline void *hooks_raw_erase(cvec_t *vec, size_t first, size_t last) {
    if (!vec->hooks.erase) {
        vec->error |= ECVEC_MISSING_HOOK_ERASE;
        return NULL;
    }
    return vec->hooks.erase(vec, first, last);
}

#endif /*CVEC_HOOKS_H*/
