//-DUSE_MACRO
//@file
#ifndef CVEC_MACRO_H
#define CVEC_MACRO_H

#ifdef USE_DUMP
#define CVEC_AS_PTR(vec) _Generic((vec), cvec_t: &(vec), cvec_t *: (vec), const cvec_t *: (vec))
#define cvec_dump(vec)   cvec_dump_with_name(CVEC_AS_PTR(vec), #vec)
#endif /*USE_DUMP*/

#define cvec_push_strliteral(vec, strliteral) cvec_push_n(vec, strliteral, sizeof(strliteral) - 1)

#endif /*CVEC_MACRO_H*/