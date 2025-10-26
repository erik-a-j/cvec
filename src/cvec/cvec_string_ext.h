#ifndef CVEC_STRING_EXT_H
#define CVEC_STRING_EXT_H
#include "cvec_base.h"
#include <string.h>

static inline int cvec_push_back_str(cvec_t *vec, const char *str) {
    size_t len_str = strlen(str);
    return cvec_push_back_n(vec, str, len_str);
}

#endif /*CVEC_STRING_EXT_H*/