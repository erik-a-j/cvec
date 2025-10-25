
#include "../cvec.h"
#include <stdio.h>

int main(void) {
    cvec_hooks_t h = cvec_hooks_init();
    cvec_t v;
    cvec_init(&v, sizeof(char), h);
    cvec_push_back_str(&v, "this is a string,\nend.");
    char *dump = cvec_dump(v);
    printf("%s\n", dump);
    free(dump);
    cvec_free(&v);
    return 0;
}