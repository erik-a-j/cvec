#include <assert.h>
#include "cvec.h"

#ifndef CVEC_NEW_T
#error "must define type"
#endif

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#ifndef CVEC_INIT_T
#define CVEC_INIT_T CONCAT(cvec_init_, CVEC_NEW_T)
#endif
#ifndef CVEC_PUSH_T
#define CVEC_PUSH_T CONCAT(cvec_push_, CVEC_NEW_T)
#endif
#ifndef CVEC_PUSHN_T
#define CVEC_PUSHN_T CONCAT(cvec_pushn_, CVEC_NEW_T)
#endif
#ifndef CVEC_STEAL_T
#define CVEC_STEAL_T CONCAT(cvec_steal_, CVEC_NEW_T)
#endif
#ifndef CVEC_AT_T
#define CVEC_AT_T CONCAT(cvec_at_, CVEC_NEW_T)
#endif
#ifndef CVEC_CAT_T
#define CVEC_CAT_T CONCAT(cvec_cat_, CVEC_NEW_T)
#endif

static inline void CVEC_INIT_T(cvec_t *vec, const cvec_hooks_t *hooks) {
    _Static_assert(sizeof(CVEC_NEW_T) > 0, "Incomplete element type"); // NOLINT
    cvec_init(vec, sizeof(CVEC_NEW_T), hooks);
}
static inline int CVEC_PUSH_T(cvec_t *vec, CVEC_NEW_T elem) {
    return cvec_push(vec, &elem);
}
static inline int CVEC_PUSHN_T(cvec_t *vec, CVEC_NEW_T elem, size_t count) {
    return cvec_pushn(vec, &elem, count);
}
static inline CVEC_NEW_T *CVEC_STEAL_T(cvec_t *vec) {
    return (CVEC_NEW_T *)cvec_steal(vec);
}
static inline CVEC_NEW_T *CVEC_AT_T(cvec_t *vec, size_t index) {
    return (CVEC_NEW_T *)cvec_at(vec, index);
}
static inline const CVEC_NEW_T *CVEC_CAT_T(const cvec_t *vec, size_t index) {
    return (const CVEC_NEW_T *)cvec_cat(vec, index);
}

#undef CVEC_NEW_T
#undef CONCAT_
#undef CONCAT
#undef CVEC_INIT_T
#undef CVEC_PUSH_T
#undef CVEC_PUSHN_T
#undef CVEC_STEAL_T
#undef CVEC_AT_T
#undef CVEC_CAT_T