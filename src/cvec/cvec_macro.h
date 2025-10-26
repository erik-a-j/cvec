#ifndef CVEC_MACRO_H
#define CVEC_MACRO_H

#define cvec_push_back_strliteral(vec, strliteral) \
    cvec_push_back_n(vec, strliteral, sizeof(strliteral)-1)

#endif /*CVEC_MACRO_H*/