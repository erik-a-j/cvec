#include "../cvec.h"
#include <stdio.h>
#include <stdarg.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
int cvec_push_back_vfmt(cvec_t *vec, const char *fmt, ...) {
    if (vec->memb_size != 1) return -1;
    va_list ap;
    va_start(ap, fmt);

    va_list ap2;
    va_copy(ap2, ap);
    int needed = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);
    if (needed < 0) {
        va_end(ap);
        return -1;
    }

    if (cvec_reserve(vec, vec->nmemb + (size_t)needed+1) != 0) {
        va_end(ap);
        return -1;
    }

    int written = vsnprintf((char*)vec->data + vec->nmemb * vec->memb_size, (size_t)needed+1, fmt, ap);
    va_end(ap);
    if (written < 0) return -1;

    vec->nmemb += (size_t)written;
    return 0;
}
#pragma clang diagnostic pop
