//-DUSE_DUMP
//-DUSE_STRING
//@file
#ifndef CVEC_STRING_H
#define CVEC_STRING_H
#include <string.h>
#include "../cvec_base.h"

static inline int cvec_push_str(cvec_t *vec, const char *str) {
    size_t len_str = strlen(str);
    return cvec_push_n(vec, str, len_str);
}

#endif /*CVEC_STRING_H*/