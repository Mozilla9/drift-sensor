/*
 * $Id$
 */

#ifndef __DATA_TYPES_H
#define __DATA_TYPES_H

#include "iolpc2364.h"
#include <stdio.h>
#include <string.h>
#include <intrinsics.h>
#include "Fmem\fmem_map.h"

/******************************************************************************/
/*                            Data types                                      */
/******************************************************************************/
typedef char                    int8_t;
typedef unsigned char           uint8_t;
typedef signed char             sint8_t;
typedef unsigned int            bool_t;
typedef short                   int16_t;
typedef unsigned short          uint16_t;
typedef signed short            sint16_t;
typedef int                     int32_t;
typedef unsigned int            uint32_t;
typedef signed int              sint32_t;
typedef long                    long32_t;
typedef unsigned long           ulong32_t;
typedef signed long             sling32_t;
typedef float                   float32_t;
typedef double                  float64_t;
typedef uint32_t (* serprint_func) (const int8_t * pData, ...);
#define TRUE_T                  (1)
#define FALSE_T                 (0)



/******************************************************************************/
/*                               Macros                                       */
/******************************************************************************/
#define BIT_SET(slovo, bit)     ((slovo) |= (1 << (bit)))
#define BIT_CLEAR(slovo, bit)   ((slovo) &= ~( 1 << (bit)))
#define BIT_TEST(slovo, bit)    ((slovo) & ( 1 << (bit)))
#define BIT_TOG(slovo, bit)     ((slovo) ^= (bit))
#define MAX(a, b)               (((a) > (b)) ? (a):(b))
#define MIN(a, b)               (((a) < (b)) ? (a):(b))
#define ARRAYSIZE(a)            (sizeof(a) / sizeof(a[0]))


#define DEBUG_PRINTF(...)       if (get_trace()) (*(get_trace()))(__VA_ARGS__)



typedef union
UNION_DWORD
{
    uint32_t  data32;
    sint32_t  sdata32;
    float32_t dataF;
    uint16_t  data16[2];
    uint8_t   data8[4];
    sint16_t  sdata16[2];
    sint8_t   sdata8[4];
} __UNION_DWORD;

#endif