#ifndef CVEC_IMPL_H
#define CVEC_IMPL_H

#include "cvec_types_impl.h"
#include "cvec_types.h"
#include "cvec_default_hooks.h"
#include "cvec_base.h"
#ifdef USE_MACRO
# include "cvec_macro.h"
#endif
#ifdef USE_STRING_EXT
# include "cvec_string_ext.h"
#endif
#ifdef USE_DUMP
# include "cvec_fmt.h"
# include "cvec_dump.h"
#elif defined(USE_FMT)
# include "cvec_fmt.h"
#endif


#endif /*CVEC_IMPL_H*/