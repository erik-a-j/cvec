#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvec.h"

static __attribute__((unused)) void *my_alloc(size_t size) {
    printf("%s called, size: %zu\n", __func__, size);
    return malloc(size);
}
static __attribute__((unused)) void *my_realloc(void *ptr, size_t size) {
    printf("%s called, ptr: %p, size: %zu\n", __func__, ptr, size);
    return realloc(ptr, size);
}
static __attribute__((unused)) void my_free(void *ptr) {
    printf("%s called, ptr: %p\n", __func__, ptr);
    free(ptr);
}
static __attribute__((unused)) void *my_memcpy(void *restrict dst, const void *restrict src, size_t n) {
    printf("%s called, dst: %p, src: %p, n: %zu\n", __func__, dst, src, n);
    return memcpy(dst, src, n);
}
static __attribute__((unused)) size_t my_grow(size_t old, size_t new, size_t size) {
    printf("%s called, old: %zu, new: %zu, size: %zu\n", __func__, old, new, size);
    return cvec_default_grow(old, new, size);
}

#define phooks(hp)                                                                                                     \
    do {                                                                                                               \
        if (hp)                                                                                                        \
            printf("hooks->alloc %p,\nhooks->realloc %p,\nhooks->free %p,\nhooks->memcpy %p,\nhooks->grow %p\n",       \
                   (void *)(uintptr_t)hp->alloc, (void *)(uintptr_t)hp->realloc, (void *)(uintptr_t)hp->free,          \
                   (void *)(uintptr_t)hp->memcpy, (void *)(uintptr_t)hp->grow);                                        \
        else                                                                                                           \
            printf("hooks == NULL\n");                                                                                 \
    } while (0)

int main(void) {
    cvec_hooks_t h;
    h.alloc   = my_alloc;
    h.realloc = my_realloc;
    h.free    = my_free;
    h.memcpy  = my_memcpy;
    h.grow    = my_grow;
    cvec_t v;
    cvec_init(&v, sizeof(char), &h);

    cvec_push_str(&v, "this is a string,\nend.");
    char *dump = cvec_dump(v);
    printf("%s\n", dump);
    free(dump);

    //End:
    cvec_free(&v);
    return 0;
}