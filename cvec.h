#ifndef CVEC_TYPE
#define CVEC_TYPE int
//#error "must define type"
#endif

#ifndef CVEC_ERROR_T
#define CVEC_ERROR_T
enum cvec_error_t {
    ECVEC_NONE = 0
   ,ECVEC_MISSING_ALLOC_FN   = (1 << 0)
   ,ECVEC_MISSING_REALLOC_FN = (1 << 1)
   ,ECVEC_MISSING_FREE_FN    = (1 << 2)
   ,ECVEC_OVERFLOW           = (1 << 3)
   ,ECVEC_ALLOC              = (1 << 4)
   ,ECVEC_REALLOC            = (1 << 5)
   ,ECVEC_GROW               = (1 << 6)
};
#endif

#define CVEC_STRINGIFY__(x) #x
#define CVEC_STRINGIFY(x) CVEC_STRINGIFY__(x)
#define CVEC_CONCAT(a,b) a ## b
#define CVEC_MAKE_NAME1(a,b) CVEC_CONCAT(a,b)
#define CVEC_MAKE_NAME(x) CVEC_MAKE_NAME1(cvec, CVEC_MAKE_NAME1(CVEC_TYPE, x))

#define CVEC_T CVEC_MAKE_NAME(_t)
#define CVEC_INIT CVEC_MAKE_NAME(_init)

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef CVEC_CUSTOM_ALLOCATORS
# include <stdlib.h>
#endif

#ifndef CVEC_FN_PTR_T
#define CVEC_FN_PTR_T
typedef void *(*alloc_fn_t)(size_t);
typedef void *(*realloc_fn_t)(void *,size_t);
typedef void  (*free_fn_t)(void *);
typedef size_t (*grow_fn_t)(size_t,size_t,size_t);
#endif /*CVEC_FN_PTR_T*/

#define CVEC_FN_T CVEC_MAKE_NAME(_fn_t)

typedef struct CVEC_FN_T {
    alloc_fn_t alloc;
    realloc_fn_t realloc;
    free_fn_t free;
    grow_fn_t grow;
} CVEC_FN_T;

#ifndef CVEC_DEFAULTS
#define CVEC_DEFAULTS
typedef struct {
    void *data;
    size_t size;
    size_t cap;

    CVEC_FN_T fn;
    enum cvec_error_t error;
    int memb_size;
} _cvec_t;

#define CVEC_GROW_ERROR 0
#define CVEC_NMEMB_MAX(memb_size) SIZE_MAX / (memb_size)
static inline size_t cvec_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size) {
    if (memb_size == 0)
        return CVEC_GROW_ERROR;
    if (old_nmemb >= new_nmemb)
        return (old_nmemb <= CVEC_NMEMB_MAX(memb_size))? old_nmemb : CVEC_GROW_ERROR;

    size_t n = old_nmemb ? old_nmemb + old_nmemb/(size_t)2 + (size_t)1 : (size_t)8;
    if (n < old_nmemb) n = CVEC_NMEMB_MAX(memb_size);
    if (n < new_nmemb) n = new_nmemb;

    if (n > CVEC_NMEMB_MAX(memb_size))
        return CVEC_GROW_ERROR;
    return n;
}

static inline void *cvec_raw_realloc(void *vec, size_t size) {
    _cvec_t *_vec = (_cvec_t*)vec;
    if (!_vec->fn.realloc) {
        _vec->error |= ECVEC_MISSING_REALLOC_FN;
        return NULL;
    }
    return _vec->fn.realloc(_vec->data, size);
}
static inline void *cvec_raw_alloc(void *vec, size_t size) {
    _cvec_t *_vec = (_cvec_t*)vec;
    if (!_vec->fn.alloc) {
        _vec->error |= ECVEC_MISSING_ALLOC_FN;
        return NULL;
    }
    return _vec->fn.alloc(size);
}
static inline void cvec_free(void *vec) {
    _cvec_t *_vec = (_cvec_t*)vec;
    if (!_vec->fn.free) {
        _vec->error |= ECVEC_MISSING_FREE_FN;
        return;
    }
    _vec->fn.free(_vec->data);
    _vec->data = NULL;
    _vec->cap = _vec->size = 0;
}
static inline int cvec_realloc(void *vec, size_t nmemb) {
    _cvec_t *v = (_cvec_t*)vec;
    if (nmemb == v->cap)
        return 0;
    if (nmemb == 0) {
        cvec_free(vec); 
        return 0;
    };

    if (v->cap == 0) {
        if (nmemb > CVEC_NMEMB_MAX(v->memb_size)) {
            v->error |= ECVEC_OVERFLOW;
            return -1;
        }
        v->data = cvec_raw_alloc(v, nmemb * v->memb_size);
        if (!v->data) {
            v->error |= ECVEC_ALLOC;
            return -1;
        }
        v->cap = nmemb;
        return 0;
    }

    size_t newcap = nmemb;
    if (nmemb > v->cap) {
        grow_fn_t grow = v->fn.grow ? v->fn.grow : cvec_grow;
        newcap = grow(v->cap, nmemb, v->memb_size);
        if (newcap == CVEC_GROW_ERROR) {
            v->error |= ECVEC_GROW;
            return -1;
        }
    }
    void *newdata = cvec_raw_realloc(vec, newcap * v->memb_size);
    if (!newdata) {
        v->error |= ECVEC_REALLOC;
        return -1;
    }
    v->data = newdata;
    v->cap = nmemb;
    if (v->size > v->cap)
        v->size = v->cap;
    return 0;
}
#endif /*CVEC_DEFAULTS*/

typedef struct {
    CVEC_TYPE *data;
    size_t size;
    size_t cap;

    CVEC_FN_T fn;
    enum cvec_error_t error;
    int memb_size;
} CVEC_T;

static inline void CVEC_INIT(CVEC_T *vec) {
    vec->data = NULL;
    vec->cap = vec->size = 0;
    vec->fn = (CVEC_FN_T){
        #ifdef CVEC_CUSTOM_ALLOCATORS
          .alloc = NULL, .realloc = NULL, .free = NULL,
        #else
          .alloc = malloc, .realloc = realloc, .free = free,
        #endif
        .grow = NULL
    };
    vec->error = ECVEC_NONE;
    vec->memb_size = sizeof(CVEC_TYPE);
}

#undef CVEC_TYPE
#undef CVEC_STRINGIFY__
#undef CVEC_STRINGIFY
#undef CVEC_CONCAT
#undef CVEC_MAKE_NAME1
#undef CVEC_MAKE_NAME
#undef CVEC_T
#undef CVEC_INIT
#undef CVEC_FN_T

/*static inline int CVEC_RESERVE(CVEC_T *vec, size_t nmemb) {
    if (nmemb == 0) return 0;
    
    size_t newcap = vec->cap ? vec->cap : 8;
    while (newcap < nmemb) {
        if (newcap * sizeof(CVEC_TYPE) > SIZE_MAX / 2)
            return -1;
        newcap <<=1;
    }
    return CVEC_REALLOC(vec, newcap);
}*/

