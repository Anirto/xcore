/* This file implements several wrappers around the basic allocation
   routines.  This is done for this reasons: so that the callers
   of these functions need not check for errors, which is easy to
   forget.  If there is not enough virtual memory for running programe,
   something is seriously wrong, and programe exits with an appropriate
   error message.
 */


#ifdef _WIN32
#define strdup _strdup
//#define assert _ASSERT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Croak the fatal memory error and bail out with non-zero exit  status.  */
static void memfatal(const char *context, size_t attempted_size)
{
    fprintf(stderr, "%s: failed to allocate %zd bytes,memory exhausted.", context, attempted_size);
    assert(0);
    exit(1);
}

void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr)
        memfatal("malloc", size);
    return ptr;
}

void *xmalloc0(size_t size)
{
    /* Using calloc can be faster than malloc+memset because some calloc
       implementations know when they're dealing with zeroed-out memory
       from the system and can avoid unnecessary memset.  */
    void *ptr = calloc(1, size);
    if (!ptr)
        memfatal("calloc", size);
    return ptr;
}

void *xcalloc(size_t num, size_t size)
{
    void *ptr = calloc(num, size);
    if (!ptr)
        memfatal("calloc", num * size);
    return ptr;
}

void *xrealloc(void *ptr, size_t newsize)
{
    void *newptr;

    /* Not all Unixes have the feature of realloc() that calling it with
       a NULL-pointer is the same as malloc(), but it is easy to
       simulate.  */
    if (ptr)
        newptr = realloc(ptr, newsize);
    else
        newptr = malloc(newsize);

    if (!newptr)
        memfatal("realloc", newsize);

    return newptr;
}

void xfree(void *ptr)
{
    /* xfree() must not be passed a NULL pointer.  */
    if (ptr) {
        free(ptr);
    }
}

void xfreenull(void **ptr)
{
    if (!ptr)
        return;

    if (*ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}
