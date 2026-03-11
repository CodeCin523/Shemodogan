# shd_idutils.h

Utilities for constructing and inspecting packed runtime identifiers in Shemodogan.

Provides macros for creating, masking, and extracting fields from handler IDs (HNDID) and sub IDs (SUBID).

⚠ These utilities are **low-level**. Incorrect usage may produce invalid runtime states. Only use if you fully understand the ID system.

---

## Tables

### Types

| | | |
|---|---|---|
| [HNDID](#hndid) | [SUBID](#subid) | [TYPE](#type) |

### Constants

| | | |
|---|---|---|
| [SHD_IDUTILS_MASK_INSTANCE](#shd_idutils_mask_instance) | [SHD_IDUTILS_MASK_PROVIDER](#shd_idutils_mask_provider) | [SHD_IDUTILS_MASK_SUB](#shd_idutils_mask_sub) |
| [SHD_IDUTILS_MASK_TYPE](#shd_idutils_mask_type) | [SHD_IDUTILS_SHIFT_INSTANCE](#shd_idutils_shift_instance) | [SHD_IDUTILS_SHIFT_PROVIDER](#shd_idutils_shift_provider) |
| [SHD_IDUTILS_SHIFT_SUB](#shd_idutils_shift_sub) | [SHD_IDUTILS_SHIFT_TYPE](#shd_idutils_shift_type) | [SHD_IDUTILS_TYPE_HND](#type) |
| [SHD_IDUTILS_TYPE_GTR](#type) | [SHD_IDUTILS_TYPE_CRT](#type) | [SHD_IDUTILS_TYPE_ACT](#type) |

### Macros / Functions

| | | |
|---|---|---|
| [SHD_IDUTILS_HHDID_INSTANCE](#shd_idutils_hhdid_instance) | [SHD_IDUTILS_HHDID_PROVIDER](#shd_idutils_hhdid_provider) | [SHD_IDUTILS_MAKE_HNDID](#shd_idutils_make_hndid) |
| [SHD_IDUTILS_MAKE_SUBID](#shd_idutils_make_subid) | [SHD_IDUTILS_SUBID_SUB](#shd_idutils_subid_sub) | [SHD_IDUTILS_SUBID_TYPE](#shd_idutils_subid_type) |

---

## Reference

### TYPE

Type constant field, 8-bit.  
Represents the type portion in SUBIDs or macros that return TYPE.  
Type: `u8`  

#### SHD_IDUTILS_TYPE_HND

Handler type constant. **Reserved, should never be used.**  

#### SHD_IDUTILS_TYPE_GTR

Getter type constant.  
Used when creating SUBIDs of type getter.  

#### SHD_IDUTILS_TYPE_CRT

Creator type constant.  
Used when creating SUBIDs of type creator.  

#### SHD_IDUTILS_TYPE_ACT

Actor type constant.  
Used when creating SUBIDs of type actor.  

---

### HNDID

Handler ID type, 16-bit.  
Used as the base ID for handlers.  
Type: `u16`  

### SUBID

Sub ID type, 32-bit.  
Constructed from HNDID, type constant, and sub field.  
Type: `u32`  

---

### SHD_IDUTILS_MAKE_HNDID

Constructs a handler ID (HNDID).

`SHD_IDUTILS_MAKE_HNDID(provider, instance)`

- `provider` (0–63) – identifies the handler provider  
- `instance` (0–255) – the handler instance index  

Layout:

pppppp_iiiiiiii

Returns: HNDID (`u16`)  

---

### SHD_IDUTILS_MAKE_SUBID

Constructs a sub ID (SUBID) from a HNDID, type constant, and sub field.

`SHD_IDUTILS_MAKE_SUBID(hndid, type, sub)`

- `hndid` – a previously created HNDID (`u16`)  
- `type` – type constant (see [TYPE](#type))  
- `sub` (0–65535) – sub index  

Layout:

pppppp_iiiiiiii_tt_ssssssssssssss

Returns: SUBID (`u32`)  

---

### SHD_IDUTILS_HHDID_PROVIDER

Extract the provider field from a HNDID.

`SHD_IDUTILS_HHDID_PROVIDER(hndid)`

Returns: `u8`  

---

### SHD_IDUTILS_HHDID_INSTANCE

Extract the instance field from a HNDID.

`SHD_IDUTILS_HHDID_INSTANCE(hndid)`

Returns: `u8`  

---

### SHD_IDUTILS_SUBID_TYPE

Extract the type field from a SUBID.

`SHD_IDUTILS_SUBID_TYPE(subid)`

Returns: TYPE (`u8`)  

---

### SHD_IDUTILS_SUBID_SUB

Extract the sub field from a SUBID.

`SHD_IDUTILS_SUBID_SUB(subid)`

Returns: `u16`  

---

### SHD_IDUTILS_MASK_INSTANCE

Mask for the instance field in HNDID (8 bits).  
Type: `u16`  

### SHD_IDUTILS_MASK_PROVIDER

Mask for the provider field in HNDID (6 bits).  
Type: `u16`  

### SHD_IDUTILS_MASK_SUB

Mask for the sub field in SUBID (16 bits).  
Type: `u16`  

### SHD_IDUTILS_MASK_TYPE

Mask for the type field in SUBID (2 bits).  
Type: `u16`  

### SHD_IDUTILS_SHIFT_INSTANCE

Bit shift for the instance field in HNDID (0).  
Type: `u8`  

### SHD_IDUTILS_SHIFT_PROVIDER

Bit shift for the provider field in HNDID (8).  
Type: `u8`  

### SHD_IDUTILS_SHIFT_SUB

Bit shift for the sub field in SUBID (18).  
Type: `u8`  

### SHD_IDUTILS_SHIFT_TYPE

Bit shift for the type field in SUBID (16).  
Type: `u8`  