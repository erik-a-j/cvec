#ifndef CVEC_H
#error "cvec.h must be included first"
#endif
#ifndef CVEC_HOOKS_H
#define CVEC_HOOKS_H

size_t default_cvec_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size);
int default_cvec_resize(cvec_t *vec, size_t nmemb);
int default_cvec_push(cvec_t *vec, const void *elem);
int default_cvec_pushn(cvec_t *vec, const void *elem, size_t count);
int default_cvec_vpushf(cvec_t *vec, const char *fmt, va_list ap) __attribute__((format(printf, 2, 0)));
int default_cvec_append(cvec_t *vec, const void *elems, size_t count);
void *default_cvec_insert(cvec_t *vec, const void *elem, size_t index);
void *default_cvec_erase(cvec_t *vec, size_t first, size_t last);

//#ifndef CVEC_CUSTOM_ALLOCATORS
//#include <stdlib.h>
//#endif
//#if !defined(CVEC_CUSTOM_MEMCPY) || !defined(CVEC_CUSTOM_MEMMOVE)
//#include <string.h>
//#endif

#define CVEC_HOOKS_INIT_OVERWRITE 0
#define CVEC_HOOKS_INIT_PARTIAL   1

static inline void cvec_hooks_init(cvec_hooks_t *hooks, int flag);
#define CVEC_HOOKS_INIT_BASE_
#include "cvec_hooks_init.h"

static inline int cvec_hookscmp(cvec_hooks_t *h1, cvec_hooks_t *h2) {
    int nneq = 0;
#define HOOKCMP(memb) if (h1->memb != h2->memb) ++nneq
    HOOKCMP(alloc);
    HOOKCMP(realloc);
    HOOKCMP(free);
    HOOKCMP(memcpy);
    HOOKCMP(memmove);
    HOOKCMP(grow);
    HOOKCMP(resize);
    HOOKCMP(push);
    HOOKCMP(pushn);
    HOOKCMP(vpushf);
    HOOKCMP(append);
    HOOKCMP(insert);
    HOOKCMP(erase);
#undef HOOKCMP
    return nneq;
}

#ifdef __STRICT_ANSI__
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
static inline int hooks_raw_vpushf(cvec_t *vec, const char *fmt, va_list ap) {
    if (!vec->hooks.vpushf) {
        vec->error |= ECVEC_MISSING_HOOK_VPUSHF;
        return -1;
    }
    return vec->hooks.vpushf(vec, fmt, ap);
}
static inline int hooks_raw_append(cvec_t *vec, const void *elems, size_t count) {
    if (!vec->hooks.append) {
        vec->error |= ECVEC_MISSING_HOOK_APPEND;
        return -1;
    }
    return vec->hooks.append(vec, elems, count);
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
#else
#define hooks_raw_alloc(vec, size)                           _hooks_raw_alloc((vec), (size))
#define hooks_raw_realloc(vec, size)                         _hooks_raw_realloc((vec), (size))
#define hooks_raw_free(vec, ptr)                             _hooks_raw_free((vec), (ptr))
#define hooks_raw_memcpy(vec, dst, src, n)                   _hooks_raw_memcpy((vec), (dst), (src), (n))
#define hooks_raw_memmove(vec, dst, src, n)                  _hooks_raw_memmove((vec), (dst), (src), (n))
#define hooks_raw_grow(vec, old_nmemb, new_nmemb, memb_size) _hooks_raw_grow((vec), (old_nmemb), (new_nmemb), (memb_size))
#define hooks_raw_resize(vec, nmemb)                         _hooks_raw_resize((vec), (nmemb))
#define hooks_raw_push(vec, elem)                            _hooks_raw_push((vec), (elem))
#define hooks_raw_pushn(vec, elem, count)                    _hooks_raw_pushn((vec), (elem), (count))
#define hooks_raw_vpushf(vec, fmt, ap)                       _hooks_raw_vpushf((vec), (fmt), (ap))
#define hooks_raw_append(vec, elems, count)                  _hooks_raw_append((vec), (elems), (count))
#define hooks_raw_insert(vec, elem, index)                   _hooks_raw_insert((vec), (elem), (index))
#define hooks_raw_erase(vec, first, last)                    _hooks_raw_erase((vec), (first), (last))

#define _hooks_raw_alloc(vec, size) ({                              \
    void *_r = NULL; cvec_t *_v = (vec);                            \
    if (!_v->hooks.alloc) {                                         \
        _v->error |= ECVEC_MISSING_HOOK_ALLOC;                      \
    } else {                                                        \
        _r = _v->hooks.alloc((size));                               \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_realloc(vec, size) ({                            \
    void *_r = NULL; cvec_t *_v = (vec);                            \
    if (!_v->hooks.realloc) {                                       \
        _v->error |= ECVEC_MISSING_HOOK_REALLOC;                    \
    } else {                                                        \
        _r = _v->hooks.realloc(_v->data, (size));                   \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_free(vec, ptr) do {                              \
    cvec_t *_v = (vec);                                             \
    if (!_v->hooks.free) {                                          \
        _v->error |= ECVEC_MISSING_HOOK_FREE;                       \
    } else {                                                        \
        _v->hooks.free(ptr);                                        \
    }                                                               \
} while (0)
#define _hooks_raw_memcpy(vec, dst, src, n) ({                      \
    void *_r = NULL; cvec_t *_v = (vec);                            \
    if (!_v->hooks.memcpy) {                                        \
        _v->error |= ECVEC_MISSING_HOOK_MEMCPY;                     \
    } else {                                                        \
        _r = _v->hooks.memcpy((dst), (src), (n));                   \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_memmove(vec, dst, src, n) ({                     \
    void *_r = NULL; cvec_t *_v = (vec);                            \
    if (!_v->hooks.memmove) {                                       \
        _v->error |= ECVEC_MISSING_HOOK_MEMMOVE;                    \
    } else {                                                        \
        _r = _v->hooks.memmove((dst), (src), (n));                  \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_grow(vec, old_nmemb, new_nmemb, memb_size) ({    \
    size_t _r = 0; cvec_t *_v = (vec);                              \
    if (!_v->hooks.grow) {                                          \
        _v->error |= ECVEC_MISSING_HOOK_GROW;                       \
    } else {                                                        \
        _r = _v->hooks.grow((old_nmemb), (new_nmemb), (memb_size)); \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_resize(vec, nmemb) ({                            \
    int _r = -1; cvec_t *_v = (vec);                                \
    if (!_v->hooks.resize) {                                        \
        _v->error |= ECVEC_MISSING_HOOK_RESIZE;                     \
    } else {                                                        \
        _r = _v->hooks.resize(_v, (nmemb));                         \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_push(vec, elem) ({                               \
    int _r = -1; cvec_t *_v = (vec);                                \
    if (!_v->hooks.push) {                                          \
        _v->error |= ECVEC_MISSING_HOOK_PUSH;                       \
    } else {                                                        \
        _r = _v->hooks.push(_v, (elem));                            \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_pushn(vec, elem, count) ({                       \
    int _r = -1; cvec_t *_v = (vec);                                \
    if (!_v->hooks.pushn) {                                         \
        _v->error |= ECVEC_MISSING_HOOK_PUSHN;                      \
    } else {                                                        \
        _r = _v->hooks.pushn(_v, (elem), (count));                  \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_vpushf(vec, fmt, ap) ({                          \
    int _r = -1; cvec_t *_v = (vec);                                \
    if (!_v->hooks.vpushf) {                                        \
        _v->error |= ECVEC_MISSING_HOOK_VPUSHF;                     \
    } else {                                                        \
		_r = _v->hooks.vpushf(_v, fmt, ap);                         \
	}                                                               \
    _r;                                                             \
})
#define _hooks_raw_append(vec, elems, count) ({                     \
    int _r = -1; cvec_t *_v = (vec);                                \
    if (!_v->hooks.append) {                                        \
        _v->error |= ECVEC_MISSING_HOOK_APPEND;                     \
    } else {                                                        \
		_r = _v->hooks.append(_v, elems, count);                    \
	}                                                               \
    _r;                                                             \
})
#define _hooks_raw_insert(vec, elem, index) ({                      \
    void *_r = NULL; cvec_t *_v = (vec);                            \
    if (!_v->hooks.insert) {                                        \
        _v->error |= ECVEC_MISSING_HOOK_INSERT;                     \
    } else {                                                        \
        _r = _v->hooks.insert(_v, (elem), (index));                 \
    }                                                               \
    _r;                                                             \
})
#define _hooks_raw_erase(vec, first, last) ({                       \
    void *_r = NULL; cvec_t *_v = (vec);                            \
    if (!_v->hooks.erase) {                                         \
        _v->error |= ECVEC_MISSING_HOOK_ERASE;                      \
    } else {                                                        \
        _r = _v->hooks.erase(_v, (first), (last));                  \
    }                                                               \
    _r;                                                             \
})

#endif /*#ifdef __STRICT_ANSI__*/
#endif /*#ifndef CVEC_HOOKS_H*/
