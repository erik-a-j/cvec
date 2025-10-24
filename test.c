
#include "cvec.h"
#include <stdio.h>

#define Dprint(v) printf(".nmemb_cap: %lu\n.nmemb: %lu\n.memb_size: %lu\n.data: %.*s\n.error: %lu\n", \
                         v.nmemb_cap, v.nmemb, v.memb_size, v.data ? (int)cvec_size(&v): 4, v.data ? (char*)v.data : "NULL", v.error)

int main(void) {
    cvec_hooks_t h = cvec_hooks_init();
    cvec_t v;
    cvec_init(&v, sizeof(char), h);
    Dprint(v);
    const char *str = "hello there";
    cvec_push_back(&v, str);
    Dprint(v);
    printf("\nsize: %lu\ncapacity: %lu\n", cvec_size(&v), cvec_capacity(&v));
    cvec_free(&v);
}