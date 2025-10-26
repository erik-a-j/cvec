#ifndef CVEC_IMPL_H
#define CVEC_IMPL_H

#include "cvec_types_impl.h"
#include "cvec_types.h"
#include "cvec_base.h"
#include "cvec_hooks.h"
#ifdef USE_MACRO
# include "cvec_macro.h"
#endif
#ifdef USE_DUMP
# include "cvec_string_ext.h"
# include "cvec_fmt.h"
# include "cvec_dump.h"
#else
# ifdef USE_STRING_EXT
# include "cvec_string_ext.h"
# endif
# ifdef USE_FMT
# include "cvec_fmt.h"
# endif
#endif


#endif /*CVEC_IMPL_H*/