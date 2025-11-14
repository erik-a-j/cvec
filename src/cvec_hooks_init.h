#ifndef CVEC_H
#error "cvec.h must be included first"
#endif

#if !defined(CVEC_HOOKS_INIT_SUFFIX) && !defined(CVEC_HOOKS_INIT_BASE_)
#error "must define name"
#endif

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#if defined(CVEC_HOOKS_INIT_BASE_) && !defined(CVEC_HOOKS_INIT_SUFFIX)
#define CVEC_HOOKS_INIT_SUFFIX
#define MAKE_NAME(a, b, c) a
#else
#define MAKE_NAME(a, b, c) CONCAT(a, CONCAT(b, c))
#endif

#ifndef CVEC_HOOKS_INIT
#define CVEC_HOOKS_INIT MAKE_NAME(cvec_hooks_init, _, CVEC_HOOKS_INIT_SUFFIX)
#endif

#ifndef CVEC_CUSTOM_ALLOCATORS
#include <stdlib.h>
#endif
#if !defined(CVEC_CUSTOM_MEMCPY) || !defined(CVEC_CUSTOM_MEMMOVE)
#include <string.h>
#endif

static inline void CVEC_HOOKS_INIT(cvec_hooks_t *hooks, int flag) {
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
        .append = default_cvec_append,
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
        if (!hooks->append) {
            hooks->append = default_hooks.append;
        }
        if (!hooks->insert) {
            hooks->insert = default_hooks.insert;
        }
        if (!hooks->erase) {
            hooks->erase = default_hooks.erase;
        }
    }
}

#ifdef CVEC_HOOKS_INIT_BASE_
#undef CVEC_HOOKS_INIT_BASE_
#endif
#undef CVEC_HOOKS_INIT_SUFFIX
#undef CONCAT_
#undef CONCAT
#undef MAKE_NAME
#undef CVEC_HOOKS_INIT