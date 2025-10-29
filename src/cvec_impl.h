#ifndef CVEC_IMPL_H
#define CVEC_IMPL_H

#include "cvec_base.h"
#include "cvec_hooks.h"
#include "cvec_types.h"
#ifdef USE_MACRO
#include "ext/cvec_macro.h"
#endif
#ifdef USE_DUMP
#include "ext/cvec_dump.h"
#include "ext/cvec_fmt.h"
#include "ext/cvec_string.h"
#else
#ifdef USE_STRING_EXT
#include "ext/cvec_string.h"
#endif
#ifdef USE_FMT
#include "ext/cvec_fmt.h"
#endif
#endif

#endif /*CVEC_IMPL_H*/