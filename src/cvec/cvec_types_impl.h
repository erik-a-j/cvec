#ifndef CVEC_TYPES_IMPL_H
#define CVEC_TYPES_IMPL_H
#include "cvec_base.h"

typedef struct cvec_internal_t {
    cvec_hooks_t hooks;
    cvec_error_t error;
} *cvec_internal;

#endif /*CVEC_TYPES_IMPL_H*/