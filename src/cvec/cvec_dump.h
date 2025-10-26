#ifndef CVEC_DUMP_H
#define CVEC_DUMP_H
#include "cvec_base.h"

#define CVEC_AS_PTR(vec)      \
    _Generic((vec),           \
        cvec_t: &(vec),       \
        cvec_t*: (vec),       \
        const cvec_t*: (vec)  \
    )
#define cvec_dump(vec) \
    cvec_dump_with_name(CVEC_AS_PTR(vec), #vec)
    
char *cvec_dump_with_name(cvec_t *vec, const char *name);

#endif /*CVEC_DUMP_H*/