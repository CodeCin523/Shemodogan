# shd_block.h

Low-level memory allocator for fixed-size blocks in Shemodogan.

⚠ **Unsafe if misused.** Metadata (bitmap) is separate from allocated memory, making it safer than the deferred stack (`shd_dfstack_t`). Still, allocations must respect the block size and pool boundaries. Only use internally or through safe wrapper functions.

Implements a block allocator (`shd_sbblock_t`) where memory is divided into fixed-size cells and allocations are tracked via a bitmap.

---

## Tables

### Types

| | | |
|---|---|---|
| [shd_sbblock_t](#shd_sbblock_t) | | |

### Constants

| | | |
|---|---|---|
| None | | |

### Macros / Functions

| | | |
|---|---|---|
| [shd_setup_sbblock_st](#shd_setup_sbblock_st) | [shd_teardown_sbblock_st](#shd_teardown_sbblock_st) | [shd_sbblock_alloc_st](#shd_sbblock_alloc_st) |
| [shd_sbblock_find_run_st](#shd_sbblock_find_run_st) | [shd_sbblock_alloc_run_st](#shd_sbblock_alloc_run_st) | [shd_sbblock_free_st](#shd_sbblock_free_st) |
| [shd_sbblock_free_run_st](#shd_sbblock_free_run_st) | [shd_sbblock_get_mt](#shd_sbblock_get_mt) | |

---

## Reference

### shd_sbblock_t

Block allocator structure.  
Manages a memory pool divided into fixed-size blocks, with allocation tracked using a bitmap.

Fields:

- `pool` – pointer to the first memory block (after bitmap)  
- `block_num` – total number of blocks in the pool  
- `bitmap_size` – size of the bitmap in bytes  
- `block_size` – size of each block in bytes  

---

### shd_setup_sbblock_st

Initialize a block allocator.

`shd_setup_sbblock_st(shd_sbblock_t *sbblock, void *pool, u32 pool_size, u16 block_size)`

- `sbblock` – pointer to allocator struct to initialize  
- `pool` – preallocated memory buffer (bitmap + blocks)  
- `pool_size` – total buffer size in bytes  
- `block_size` – size of each block  

Behavior: calculates bitmap size, clears bitmap (all blocks free), and sets up allocator fields.

---

### shd_teardown_sbblock_st

Reset a block allocator to zeroed state.

`shd_teardown_sbblock_st(shd_sbblock_t *sbblock)`

- `sbblock` – allocator to reset  

---

### shd_sbblock_alloc_st

Allocate a single free block.

`u32 shd_sbblock_alloc_st(shd_sbblock_t *sbblock)`

- `sbblock` – allocator to allocate from  

Returns: block index (`u32`) or `u32_MAX` if no free block available.

---

### shd_sbblock_find_run_st

Find a contiguous run of free blocks.

`u32 shd_sbblock_find_run_st(shd_sbblock_t *sbblock, u32 count)`

- `sbblock` – allocator to search  
- `count` – number of contiguous blocks required  

Returns: start index of the run, or `u32_MAX` if no run found.

---

### shd_sbblock_alloc_run_st

Allocate a contiguous run of blocks at a specific start index.

`u32 shd_sbblock_alloc_run_st(shd_sbblock_t *sbblock, u32 idx, u32 count)`

- `sbblock` – allocator to allocate from  
- `idx` – start index of run  
- `count` – number of blocks to allocate  

Returns: start index if successful, `u32_MAX` if allocation fails.

---

### shd_sbblock_free_st

Free a single allocated block.

`shd_sbblock_free_st(shd_sbblock_t *sbblock, u32 idx)`

- `sbblock` – allocator containing the block  
- `idx` – index of the block to free  

Marks the block as free in the bitmap.

---

### shd_sbblock_free_run_st

Free a contiguous run of allocated blocks.

`shd_sbblock_free_run_st(shd_sbblock_t *sbblock, u32 idx, u32 count)`

- `sbblock` – allocator containing the blocks  
- `idx` – start index of the run  
- `count` – number of blocks to free  

---

### shd_sbblock_get_mt

Get pointer to memory of a specific block.

`void* shd_sbblock_get_mt(shd_sbblock_t *sbblock, u32 idx)`

- `sbblock` – allocator  
- `idx` – index of block  

Returns: pointer to block memory, or `NULL` if index is out of bounds.

---

### Notes

- **Safer than stack allocator:** metadata is separate from payload (bitmap precedes memory blocks).  
- **Bitmap-managed allocation:** All blocks tracked via bitmap.  
- **Caller-managed alignment:** Alignment is the responsibility of the caller.  
- **Single-threaded only:** `_st` functions are **not thread-safe**.  
- **Supports single and contiguous run allocations**.  
- **Recommended usage:** internal library memory only; wrap for safe external usage.  