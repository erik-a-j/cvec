#include "cvec_impl.h"

#ifndef USE_MACRO
#define cvec_push_back_strliteral(vec, strliteral) \
    cvec_push_back_n(vec, strliteral, sizeof(strliteral)-1)
#endif

char *cvec_dump_with_name(cvec_t *vec, const char *name) {
    char *dump = NULL;
    cvec_t d;
    cvec_init(&d, sizeof(char), vec->_->hooks);
    cvec_reserve(&d, 1024);
    
    if (cvec_push_back_str(&d, name) != 0) goto End;
    if (cvec_push_back_strliteral(&d, ": {\n") != 0) goto End;
    if (cvec_push_back_fmt(&d, "  .memb_size: %zu,\n", vec->memb_size) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  .nmemb_cap: %zu,\n", vec->nmemb_cap) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  .nmemb: %zu,\n", vec->nmemb) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  ._->error: %zu,\n", vec->_->error) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  ._->hooks.alloc: %p,\n", vec->_->hooks.alloc) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  ._->hooks.realloc: %p,\n", vec->_->hooks.realloc) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  ._->hooks.free: %p,\n", vec->_->hooks.free) != 0) goto End;
    if (cvec_push_back_fmt(&d, "  ._->hooks.grow: %p,\n", vec->_->hooks.grow) != 0) goto End;
    if (vec->memb_size == 1) {
        if (cvec_push_back_fmt(&d, "  .data:\n  [\n%.*s", vec->nmemb, vec->data) != 0) goto End;
        if (*(char*)cvec_at(vec, vec->nmemb-1) != '\n') {
            if (cvec_push_back_strliteral(&d, "\n") != 0) goto End;
        }
        if (cvec_push_back_strliteral(&d, "  ],\n") != 0) goto End;
    }
    if (cvec_push_back_strliteral(&d, "}\0") != 0) goto End;
    
    dump = cvec_steal(&d);
End:
    cvec_free(&d);
    return dump;
}

#if defined(cvec_push_back_strliteral) && !defined(USE_MACRO)
# undef cvec_push_back_strliteral
#endif