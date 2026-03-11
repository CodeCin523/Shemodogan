# shd_num.h

Fixed-width numeric types for the Shemodogan runtime.

Provides consistent integer and floating-point types across platforms, along with min/max constants for each type.

⚠ These are low-level definitions. They form the basis for all numeric operations in the library. Misuse is unlikely but may cause platform-specific inconsistencies if altered.

---

## Tables

### Types

| | | |
|---|---|---|
| [i8](#i8)   | [i16](#i16) | [i32](#i32) |
| [i64](#i64) | [u8](#u8)   | [u16](#u16) |
| [u32](#u32) | [u64](#u64) | [f32](#f32) |
| [f64](#f64) |             |             |

### Constants

| | | |
|---|---|---|
| [i8_MIN](#i8_min)   | [i8_MAX](#i8_max)   | [i16_MIN](#i16_min) |
| [i16_MAX](#i16_max) | [i32_MIN](#i32_min) | [i32_MAX](#i32_max) |
| [i64_MIN](#i64_min) | [i64_MAX](#i64_max) | [u8_MIN](#u8_min)   |
| [u8_MAX](#u8_max)   | [u16_MIN](#u16_min) | [u16_MAX](#u16_max) |
| [u32_MIN](#u32_min) | [u32_MAX](#u32_max) | [u64_MIN](#u64_min) |
| [u64_MAX](#u64_max) |                     |                     |

---

## Reference

### i8

8-bit signed integer.  
Type: `int8_t`  

### i16

16-bit signed integer.  
Type: `int16_t`  

### i32

32-bit signed integer.  
Type: `int32_t`  

### i64

64-bit signed integer.  
Type: `int64_t`  

### u8

8-bit unsigned integer.  
Type: `uint8_t`  

### u16

16-bit unsigned integer.  
Type: `uint16_t`  

### u32

32-bit unsigned integer.  
Type: `uint32_t`  

### u64

64-bit unsigned integer.  
Type: `uint64_t`  

### f32

32-bit floating-point (IEEE-754 binary32).  
Type: `float`  

### f64

64-bit floating-point (IEEE-754 binary64).  
Type: `double`  

---

### i8_MIN

Minimum value for `i8`.  
Value: `INT8_MIN`  

### i8_MAX

Maximum value for `i8`.  
Value: `INT8_MAX`  

### i16_MIN

Minimum value for `i16`.  
Value: `INT16_MIN`  

### i16_MAX

Maximum value for `i16`.  
Value: `INT16_MAX`  

### i32_MIN

Minimum value for `i32`.  
Value: `INT32_MIN`  

### i32_MAX

Maximum value for `i32`.  
Value: `INT32_MAX`  

### i64_MIN

Minimum value for `i64`.  
Value: `INT64_MIN`  

### i64_MAX

Maximum value for `i64`.  
Value: `INT64_MAX`  

### u8_MIN

Minimum value for `u8`.  
Value: `0`  

### u8_MAX

Maximum value for `u8`.  
Value: `UINT8_MAX`  

### u16_MIN

Minimum value for `u16`.  
Value: `0`  

### u16_MAX

Maximum value for `u16`.  
Value: `UINT16_MAX`  

### u32_MIN

Minimum value for `u32`.  
Value: `0`  

### u32_MAX

Maximum value for `u32`.  
Value: `UINT32_MAX`  

### u64_MIN

Minimum value for `u64`.  
Value: `0`  

### u64_MAX

Maximum value for `u64`.  
Value: `UINT64_MAX`  