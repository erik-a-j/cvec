#ifndef CVEC_HOOKS_H
#define CVEC_HOOKS_H
#include "cvec_api.h"
#include "cvec_types.h"

//cvec_default_grow
CVEC_API size_t cvec_default_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

#ifndef CVEC_CUSTOM_ALLOCATORS
#include <stdlib.h>
#endif
#ifndef CVEC_CUSTOM_MEMCPY
#include <string.h>
#endif

//CVEC_HOOKS_INIT_$FLAG(start)
#define CVEC_HOOKS_INIT_OVERWRITE 0 /**< Will overwrite all members of `hooks` */
#define CVEC_HOOKS_INIT_PARTIAL   1 /**< Will overwrite all `NULL` members of `hooks` */
//CVEC_HOOKS_INIT_$FLAG(end)

//cvec_hooks_init
static inline void cvec_hooks_init(cvec_hooks_t *hooks, int flag) {
    cvec_hooks_t default_hooks = {
#ifdef CVEC_CUSTOM_ALLOCATORS
        .alloc   = NULL,
        .realloc = NULL,
        .free    = NULL,
#else
        .alloc   = malloc,
        .realloc = realloc,
        .free    = free,
#endif
#ifdef CVEC_CUSTOM_MEMCPY
        .memcpy = NULL,
#else
        .memcpy = memcpy,
#endif
        .grow = cvec_default_grow,
    };

    if (flag == CVEC_HOOKS_INIT_OVERWRITE) {
        *hooks = default_hooks;
    } else if (flag == CVEC_HOOKS_INIT_PARTIAL) {
        if (!hooks->alloc) { hooks->alloc = default_hooks.alloc; }
        if (!hooks->realloc) { hooks->realloc = default_hooks.realloc; }
        if (!hooks->free) { hooks->free = default_hooks.free; }
        if (!hooks->memcpy) { hooks->memcpy = default_hooks.memcpy; }
        if (!hooks->grow) { hooks->grow = default_hooks.grow; }
    }
}

#endif /*CVEC_HOOKS_H*/