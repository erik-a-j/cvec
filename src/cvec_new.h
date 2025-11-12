#ifndef CVEC_H
#error "cvec.h must be included first"
#endif

#if !defined(CVEC_T) && !defined(CVEC_T_BASE_)
#error "must define type"
#endif

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#if defined(CVEC_T_BASE_) && !defined(CVEC_T)
#define CVEC_T void
#define MAKE_NAME(a, b, c) a
#else
#define MAKE_NAME(a, b, c) CONCAT(a, CONCAT(b, c))
#endif

#ifndef CVEC_INIT_T
#define CVEC_INIT_T MAKE_NAME(cvec_init, _, CVEC_T)
#endif
#ifndef CVEC_PUSH_T
#define CVEC_PUSH_T MAKE_NAME(cvec_push, _, CVEC_T)
#endif
#ifndef CVEC_PUSHN_T
#define CVEC_PUSHN_T MAKE_NAME(cvec_pushn, _, CVEC_T)
#endif
#ifndef CVEC_INSERT_T
#define CVEC_INSERT_T MAKE_NAME(cvec_insert, _, CVEC_T)
#endif
#ifndef CVEC_ERASE_T
#define CVEC_ERASE_T MAKE_NAME(cvec_erase, _, CVEC_T)
#endif
#ifndef CVEC_DATA_T
#define CVEC_DATA_T MAKE_NAME(cvec_data, _, CVEC_T)
#endif
#ifndef CVEC_CDATA_T
#define CVEC_CDATA_T MAKE_NAME(cvec_cdata, _, CVEC_T)
#endif
#ifndef CVEC_FRONT_T
#define CVEC_FRONT_T MAKE_NAME(cvec_front, _, CVEC_T)
#endif
#ifndef CVEC_CFRONT_T
#define CVEC_CFRONT_T MAKE_NAME(cvec_cfront, _, CVEC_T)
#endif
#ifndef CVEC_BACK_T
#define CVEC_BACK_T MAKE_NAME(cvec_back, _, CVEC_T)
#endif
#ifndef CVEC_CBACK_T
#define CVEC_CBACK_T MAKE_NAME(cvec_cback, _, CVEC_T)
#endif
#ifndef CVEC_AT_T
#define CVEC_AT_T MAKE_NAME(cvec_at, _, CVEC_T)
#endif
#ifndef CVEC_CAT_T
#define CVEC_CAT_T MAKE_NAME(cvec_cat, _, CVEC_T)
#endif
#ifndef CVEC_STEAL_T
#define CVEC_STEAL_T MAKE_NAME(cvec_steal, _, CVEC_T)
#endif

#ifndef CVEC_T_BASE_
static inline void CVEC_INIT_T(cvec_t *vec, const cvec_hooks_t *hooks) {
    _Static_assert(sizeof(CVEC_T) > 0, "Incomplete element type"); // NOLINT
    cvec_init(vec, sizeof(CVEC_T), hooks);
}
static inline int CVEC_PUSH_T(cvec_t *vec, CVEC_T elem) {
    return cvec_push(vec, &elem);
}
static inline int CVEC_PUSHN_T(cvec_t *vec, CVEC_T elem, size_t count) {
    return cvec_pushn(vec, &elem, count);
}
static inline CVEC_T *CVEC_INSERT_T(cvec_t *vec, CVEC_T elem, size_t index) {
    return (CVEC_T *)cvec_insert(vec, &elem, index);
}
static inline CVEC_T *CVEC_ERASE_T(cvec_t *vec, size_t first, size_t last) {
    return (CVEC_T *)cvec_erase(vec, first, last);
}
#endif
static inline CVEC_T *CVEC_DATA_T(cvec_t *vec) {
    return (CVEC_T *)vec->data;
}
static inline const CVEC_T *CVEC_CDATA_T(const cvec_t *vec) {
    return (const CVEC_T *)vec->data;
}
static inline CVEC_T *CVEC_FRONT_T(cvec_t *vec) {
    return (vec->nmemb > 0) ? (CVEC_T *)vec->data : NULL;
}
static inline const CVEC_T *CVEC_CFRONT_T(const cvec_t *vec) {
    return (vec->nmemb > 0) ? (const CVEC_T *)vec->data : NULL;
}
static inline CVEC_T *CVEC_BACK_T(cvec_t *vec) {
    void *p = (vec->nmemb > 0) ? (char *)vec->data + (vec->nmemb - 1) * vec->memb_size : NULL;
    return (CVEC_T *)p;
}
static inline const CVEC_T *CVEC_CBACK_T(const cvec_t *vec) {
    const void *p = (vec->nmemb > 0) ? (const char *)vec->data + (vec->nmemb - 1) * vec->memb_size : NULL;
    return (const CVEC_T *)p;
}
static inline CVEC_T *CVEC_AT_T(cvec_t *vec, size_t index) {
    if (index >= vec->nmemb || (vec->memb_size != 0 && index > SIZE_MAX / vec->memb_size)) {
        return NULL;
    }
    void *p = (char *)vec->data + index * vec->memb_size;
    return (CVEC_T *)p;
}
static inline const CVEC_T *CVEC_CAT_T(const cvec_t *vec, size_t index) {
    if (index >= vec->nmemb || (vec->memb_size != 0 && index > SIZE_MAX / vec->memb_size)) {
        return NULL;
    }
    const void *p = (const char *)vec->data + index * vec->memb_size;
    return (const CVEC_T *)p;
}
static inline CVEC_T *CVEC_STEAL_T(cvec_t *vec) {
    CVEC_T *stolen = vec->data;
    vec->data = NULL;
    vec->nmemb_cap = vec->nmemb = 0;
    return stolen;
}

#ifdef CVEC_T_BASE_
#undef CVEC_T_BASE_
#endif
#undef CVEC_T
#undef CONCAT_
#undef CONCAT
#undef MAKE_NAME
#undef CVEC_INIT_T
#undef CVEC_PUSH_T
#undef CVEC_PUSHN_T
#undef CVEC_INSERT_T
#undef CVEC_ERASE_T
#undef CVEC_DATA_T
#undef CVEC_CDATA_T
#undef CVEC_FRONT_T
#undef CVEC_CFRONT_T
#undef CVEC_BACK_T
#undef CVEC_CBACK_T
#undef CVEC_AT_T
#undef CVEC_CAT_T
#undef CVEC_STEAL_T
