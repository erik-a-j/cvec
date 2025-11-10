#ifndef CVEC_HOOKS_H
#define CVEC_HOOKS_H
#include "cvec_types.h"

size_t cvec_default_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

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
        .grow = cvec_default_grow,
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
    }
}

#endif /*CVEC_HOOKS_H*/
