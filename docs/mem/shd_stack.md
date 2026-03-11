# shd_stack.h

Low-level deferred stack utility for Shemodogan.

⚠ **Extremely unsafe.** Direct usage can easily corrupt memory if rules are not respected. Only use internally or through safe wrapper functions.

Implements a single deferred stack structure (`shd_dfstack_t`) for managing allocations with optional deferred free. All memory alignment is the responsibility of the caller.

---

## Tables

### Types

| | | |
|---|---|---|
| [shd_dfmeta](#shd_dfmeta) | [shd_dfstack_t](#shd_dfstack_t) | |

### Constants

| | | |
|---|---|---|
| [SHD_ALIGN_UP](#shd_align_up) | [SHD_MEMDFFLAGS](#shd_memdfflags) | [SHD_MEMDFVALUE](#shd_memdfvalue) |

### Macros / Functions

| | | |
|---|---|---|
| [shd_setup_dfstack_st](#shd_setup_dfstack_st) | [shd_teardown_dfstack_st](#shd_teardown_dfstack_st) | [shd_dfstack_push_st](#shd_dfstack_push_st) |
| [shd_dfstack_pop_st](#shd_dfstack_pop_st) | [shd_dfstack_clear_st](#shd_dfstack_clear_st) | |

---

## Reference

### shd_dfmeta

Metadata struct for deferred stack allocations.  
Contains distance to previous and next meta, with deferred-free flags.  

Size: 8 bytes  

Fields:

- `lcount` – distance in bytes to previous meta; top bit flags deferred free  
- `ncount` – distance in bytes to next meta  

---

### shd_dfstack_t

Deferred stack representation.  
Holds a memory pool, current top pointer, and total pool size in bytes.  

Fields:

- `pool` – pointer to memory pool start  
- `last` – current top meta  
- `size` – total pool size in bytes  

---

### SHD_ALIGN_UP

Align a value `x` up to the next multiple of `a`.

`SHD_ALIGN_UP(x, a)`

- `x` – value to align  
- `a` – alignment (must be power of two)  

Returns: aligned value (`uintptr_t`)  

---

### SHD_MEMDFFLAGS

Deferred free flag for meta `lcount`.  
Type: `u32`  

---

### SHD_MEMDFVALUE

Mask for meta `lcount` to extract usable span value.  
Type: `u32`  

---

### shd_setup_dfstack_st

Initialize a deferred stack.

`shd_setup_dfstack_st(shd_dfstack_t *vstack, void *pool, u32 size)`

- `vstack` – pointer to stack struct to initialize  
- `pool` – preallocated memory block  
- `size` – size of memory block  

---

### shd_teardown_dfstack_st

Reset a deferred stack to zeroed state.

`shd_teardown_dfstack_st(shd_dfstack_t *vstack)`

---

### shd_dfstack_push_st

Push allocation on the deferred stack.

`void* shd_dfstack_push_st(shd_dfstack_t *vstack, u32 size)`

- `vstack` – stack to push on  
- `size` – allocation size  

Returns: pointer to payload or `NULL` if overflow  

---

### shd_dfstack_pop_st

Pop allocation from the deferred stack.

`shd_dfstack_pop_st(shd_dfstack_t *vstack, void *addr)`

- `vstack` – stack to pop from  
- `addr` – address to pop; if `NULL`, collapses top allocation  

Handles deferred free if `addr` is not at the top.  

---

### shd_dfstack_clear_st

Clear the stack completely, collapsing all allocations.

`shd_dfstack_clear_st(shd_dfstack_t *vstack)`

- `vstack` – stack to clear  

---

### Notes

- **Unsafe:** Direct usage may corrupt memory.  
- **Caller-managed alignment:** Memory alignment is the caller’s responsibility.  
- **Deferred free:** Allows marking allocations for later collapse.  
- **Single-threaded only:** All `_st` functions are **not thread-safe**.  
- **Other stack types may be implemented later.**  
- **Recommended usage:** internal library memory only; wrap for safe external usage.  