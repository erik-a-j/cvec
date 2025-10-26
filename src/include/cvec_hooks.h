#ifndef CVEC_HOOKS_H
#define CVEC_HOOKS_H
#include "cvec_api.h"
#include "cvec_types.h"

CVEC_API size_t cvec_default_grow(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

#ifndef CVEC_CUSTOM_ALLOCATORS
# include <stdlib.h>
#endif
#ifndef CVEC_CUSTOM_MEMCPY
# include <string.h>
#endif

/**
 * @brief 
 * `cvec_*` functions requires that __All__ of the __hooks__ must be valid `pointer to function`.  
 * If none of the `CVEC_CUSTOM*` preprocessor definitions are defined
 * prior to including `"cvec.h"`, this function will set each member of returned
 * `cvec_hooks_t struct` to a default valid `pointer to function`.
 * 
 * @attention
 * - All `allocator hooks` will be initialized to `NULL` if `CVEC_CUSTOM_ALLOCATORS`
 * is defined prior to including `"cvec.h"`, else `<stdlib.h>` will get included
 * and `allocator hooks` will be initialized to `malloc`, `realloc` and `free`.
 * @attention
 * - The `memcpy hook` will be initialized to `NULL` if `CVEC_CUSTOM_MEMCPY`
 * is defined prior to including `"cvec.h"`, else `<string.h>` will get included
 * and `memcpy hook` will be initialized to `memcpy`. 
 * @attention
 * - The `grow hook` will always be initialized to `cvec_default_grow`.
 * @return `struct cvec_hooks_t`
 * @note see `struct cvec_hooks_t`
 */
static inline cvec_hooks_t cvec_hooks_init(void) {
    cvec_hooks_t h = {
        #ifdef CVEC_CUSTOM_ALLOCATORS
          .alloc = NULL, .realloc = NULL, .free = NULL,
        #else
          .alloc = malloc, .realloc = realloc, .free = free,
        #endif
        #ifdef CVEC_CUSTOM_MEMCPY
          .memcpy = NULL,
        #else
          .memcpy = memcpy,
        #endif
        .grow = cvec_default_grow
    };
    return h;
}
CVEC_API cvec_hooks_t *cvec_hooks_get(cvec_t *vec);

#endif /*CVEC_HOOKS_H*/