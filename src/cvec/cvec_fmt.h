#ifndef CVEC_FMT_H
#define CVEC_FMT_H
#include "cvec_base.h"
#include <stdio.h>
#include <stdarg.h>

int cvec_push_back_fmt(cvec_t *vec, const char *fmt, ...);

#endif /*CVEC_FMT_H*/