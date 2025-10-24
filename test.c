//#define CVEC_CUSTOM_ALLOCATORS
#define CVEC_TYPE char

#include "cvec.h"
#include <stdio.h>

#define Dprint(v) printf(".cap: %lu\n.size: %lu\n.data: %s\n.error: %d\n", \
                         v.cap, v.size, v.data ? (char*)v.data : "NULL", v.error)

int main(void) {
    cvecchar_t v;
    
    cvecchar_init(&v);
    Dprint(v);
    cvecchar_reserve(&v, 16);
    Dprint(v);
    cvec_free(&v);
    Dprint(v);
}