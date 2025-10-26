#ifndef CVEC_TYPES_H
#define CVEC_TYPES_H
#include <stddef.h>

enum cvec_error_t {
    ECVEC_NONE               = 0u
   ,ECVEC_MISSING_ALLOC_FN   = (1u << 0)
   ,ECVEC_MISSING_REALLOC_FN = (1u << 1)
   ,ECVEC_MISSING_FREE_FN    = (1u << 2)
   ,ECVEC_MISSING_GROW_FN    = (1u << 3)
   ,ECVEC_MISSING_MEMCPY_FN  = (1u << 4)
   ,ECVEC_OVERFLOW           = (1u << 5)
   ,ECVEC_ALLOC              = (1u << 6)
   ,ECVEC_REALLOC            = (1u << 7)
   ,ECVEC_RESIZE             = (1u << 8)
   ,ECVEC_GROW               = (1u << 9)
};
typedef size_t cvec_error_t;

typedef void *(*alloc_fn_t)(size_t size);
typedef void *(*realloc_fn_t)(void *ptr, size_t size);
typedef void  (*free_fn_t)(void *ptr);
typedef void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n);
typedef size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size);

/**
 * @brief see struct definition for info
 * @note see `cvec_hooks_t cvec_hooks_init(void)`
 */
typedef struct cvec_hooks_t {

    /** @brief _typedef_ : `void *(*alloc_fn_t)(size_t size)`
     *  @param (size_t)size size in bytes
     *  @return ___on success:___ pointer to start of allocated memory, ___on fail:___ NULL
     */
    alloc_fn_t   alloc;

    /** @brief _typedef_ : `void *(*realloc_fn_t)(void *ptr, size_t size)`
     *  @param (void*)ptr pointer to start of allocated memory
     *  @param (size_t)size size in bytes
     *  @return ___on success:___ pointer to start of reallocated memory, ___on fail:___ NULL
     */
    realloc_fn_t realloc;

    /** @brief _typedef_ : `void (*free_fn_t)(void *ptr)`
     *  @param (void*)ptr pointer to start of allocated memory
     */
    free_fn_t    free;

    /** @brief _typedef_ : `void *(*memcpy_fn_t)(void *restrict dst, const void *restrict src, size_t n)`
     *  @param (void*restrict)dst pointer to destination
     *  @param (void*restrict)src pointer to source
     *  @param (size_t)n size in bytes
     *  @return ___on success:___ pointer to start of dst, ___on fail:___ NULL
     */ 
    memcpy_fn_t  memcpy;

    /** @brief _typedef_ : `size_t (*grow_fn_t)(size_t old_nmemb, size_t new_nmemb, size_t memb_size)`
     *  @param (size_t)old_nmemb previous number of elements
     *  @param (size_t)new_nmemb new number of elements
     *  @param (size_t)memb_size size of one element in bytes
     *  @return ___on success:___ value above 0, ___on fail:___ 0
     */
    grow_fn_t    grow;
} cvec_hooks_t;

typedef struct cvec_internal_t *cvec_internal;

/**
 * @brief see struct definition for info
 */
typedef struct cvec_t {
    void   *data;       /*pointer to underlying data*/
    size_t  nmemb_cap;  /*number of members capacity*/
    size_t  nmemb;      /*number of members currently used*/
    size_t  memb_size;  /*size of one member in bytes*/
    //cvec_error_t error; /*error flags, see definition of `enum cvec_error_t`*/
    //cvec_hooks_t fn;    /*hooks for fine control over how instance of cvec_t is handled, see `struct cvec_hooks_t`*/
    cvec_internal _;
} cvec_t;

#endif /*CVEC_TYPES_H*/