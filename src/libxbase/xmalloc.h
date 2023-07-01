#ifndef XMALLOC_H
#define XMALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define alloca _alloca
#endif

#ifdef __cplusplus
extern "C" {
#endif

void *xmalloc(size_t size);
void *xmalloc0(size_t size);
void *xcalloc(size_t num, size_t size);
void *xrealloc(void *ptr, size_t newsize);
void xfreenull(void **ptr);

#ifdef __cplusplus
}
#endif

#define xnew(type) ((type *)xmalloc(sizeof(type)))
#define xnew0(type) ((type *)xmalloc0(sizeof(type)))
#define xnew_array(type, len) ((type *)xmalloc((len) * sizeof(type)))
#define xnew0_array(type, len) ((type *)xmalloc0((len) * sizeof(type)))
#define alloca_array(type, size) ((type *)alloca((size) * sizeof(type)))

#define xfree(p)        \
    do {                \
        if (p) {        \
            free(p);    \
            (p) = NULL; \
        }               \
    } while (0)

#endif
