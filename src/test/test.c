#include <stdlib.h>
#include <string.h>
#include "cvec.h"
#include <stdio.h>
#include <stdint.h>

#define phooks(hp) \
printf(#hp"->alloc %p,\n"#hp"->realloc %p,\n"#hp"->free %p,\n"#hp"->memcpy %p,\n"#hp"->grow %p\n", \
       (void*)(uintptr_t)hp->alloc, (void*)(uintptr_t)hp->realloc, (void*)(uintptr_t)hp->free, (void*)(uintptr_t)hp->memcpy, (void*)(uintptr_t)hp->grow)

int main(void) {
    cvec_hooks_t h = cvec_hooks_init();
    cvec_t v;
    if (cvec_init(&v, sizeof(char), h) != 0)
        return 1;
    
    cvec_push_back_str(&v, "this is a string,\nend.");
    char *dump = cvec_dump(v);
    printf("%s\n", dump);
    free(dump);

    cvec_hooks_t *hooks = cvec_hooks_get(&v);
    phooks(hooks);

    cvec_free(&v);
    return 0;
}