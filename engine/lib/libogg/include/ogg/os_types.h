#ifndef __CONFIG_TYPES_H__
#define __CONFIG_TYPES_H__



#include <stdint.h>

typedef int16_t ogg_int16_t;
typedef uint16_t ogg_uint16_t;
typedef int32_t ogg_int32_t;
typedef uint32_t ogg_uint32_t;
typedef int64_t ogg_int64_t;

#define _ogg_free(X) free(X)
#define _ogg_malloc(X) malloc(X)
#define _ogg_calloc(X, Y) calloc(X, Y)
#define _ogg_realloc(X, Y) realloc(X, Y)



#endif
