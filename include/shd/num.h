#ifndef __SHD_NUM_H__
#define __SHD_NUM_H__

#include <stdint.h>
#include <limits.h>
#include <float.h>

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       f32;
typedef double      f64;

#define i8_MIN   INT8_MIN
#define i8_MAX   INT8_MAX
#define i16_MIN  INT16_MIN
#define i16_MAX  INT16_MAX
#define i32_MIN  INT32_MIN
#define i32_MAX  INT32_MAX
#define i64_MIN  INT64_MIN
#define i64_MAX  INT64_MAX

#define u8_MIN   0u
#define u8_MAX   UINT8_MAX
#define u16_MIN  0u
#define u16_MAX  UINT16_MAX
#define u32_MIN  0u
#define u32_MAX  UINT32_MAX
#define u64_MIN  0u
#define u64_MAX  UINT64_MAX

/* #region Sanity checks # */
/* ####################### */
_Static_assert(sizeof(i8)  == 1, "i8 must be 1 byte");
_Static_assert(sizeof(i16) == 2, "i16 must be 2 bytes");
_Static_assert(sizeof(i32) == 4, "i32 must be 4 bytes");
_Static_assert(sizeof(i64) == 8, "i64 must be 8 bytes");

_Static_assert(sizeof(f32) == 4, "f32 must be 4 bytes (IEEE-754 binary32)");
_Static_assert(sizeof(f64) == 8, "f64 must be 8 bytes (IEEE-754 binary64)");

/* Floating-point format (Rust FFI contract) */
_Static_assert(FLT_RADIX == 2,      "Floating point must be base-2");
_Static_assert(FLT_MANT_DIG == 24,  "f32 must be IEEE-754 binary32");
_Static_assert(DBL_MANT_DIG == 53,  "f64 must be IEEE-754 binary64");
/* #endregion            # */
/* ####################### */

#endif /* __SHD_NUM_H__ */
