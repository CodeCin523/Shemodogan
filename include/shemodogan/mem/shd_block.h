#ifndef __SHD_BLOCK_H__
#define __SHD_BLOCK_H__

#include "../shd_num.h"


/* ================================================================================ */
/*  TYPEDEF                                                                         */
/* ================================================================================ */

// self-bitmap block allocator
typedef struct shd_sbblock {
    u8 *pool;       // start of the buffer, after the bitmap
    u32 block_num;  // total cell count
    u16 bitmap_size;// size of bitmap buffer
    u16 block_size; // size of single cell
} shd_sbblock_t;


/* ================================================================================ */
/*  SELF_BITMAP_BLOCK                                                               */
/* ================================================================================ */

static inline void shd_setup_sbblock_st(shd_sbblock_t *sbblock, void *pool, u32 pool_size, u16 block_size) {
    if (!sbblock || !pool || block_size == 0) return;


    u32 c0 = pool_size / block_size;         // CellCount0
    u32 b0 = (c0 + 7) / 8;                  // BitmapByte0
    u32 c1 = (pool_size - b0) / block_size;  // CellCount1
    u32 b1 = (c1 + 7) / 8;                  // BitmapByte1

    sbblock->pool = (u8 *)pool + b1;
    sbblock->block_num = c1;
    sbblock->bitmap_size = b1;
    sbblock->block_size = block_size;

    // clear bitmap (all cells free)
    u8 *bitmap = (u8*)pool;
    for (u32 i = 0; i < b1; ++i) {
        bitmap[i] = 0;
    }
}
static inline void shd_teardown_sbblock_st(shd_sbblock_t *sbblock) {
    if(!sbblock) return;

    sbblock->pool = 0;
    sbblock->bitmap_size = 0;
    sbblock->block_size = 0;
    sbblock->block_num = 0;
}

// maybe shd_sbblock_alloc_mt
static inline u32 shd_sbblock_alloc_st(shd_sbblock_t *sbblock) {
    u8 *bitmap = sbblock->pool - sbblock->bitmap_size;

    for(u32 i = 0; i < sbblock->bitmap_size; ++i) {
        if(bitmap[i] == 0xff) continue;

        // Can be replaced by a single 
        for(u32 j = 0; j < 8; ++j) {
            // can probably start
            if((bitmap[i] >> j & 1) != 1) {
                bitmap[i] |= (1 << j);
                return i * 8 + j;
            }
        }
    }
    return u32_MAX;
}

static inline u32 shd_sbblock_find_run_st(shd_sbblock_t *sbblock, u32 count) {
    u32 run_start = 0, run_len = 0;
    u8 *bitmap = sbblock->pool - sbblock->bitmap_size;

    for (u32 i = 0; i < sbblock->bitmap_size * 8; ++i) {
        u32 idx_byte = i / 8;
        u32 idx_bit = i % 8;

        if((bitmap[idx_byte] >> idx_bit & 1) != 1) {
            if(run_len == 0) run_start = i;
            ++run_len;
            if(run_len == count) return run_start;
        } else {
            run_len = 0;
        }
    }

    return u32_MAX;
}
static inline u32 shd_sbblock_alloc_run_st(shd_sbblock_t *sbblock, u32 idx, u32 count) {
    if (!sbblock || idx + count > sbblock->block_num) return u32_MAX;

    u8 *bitmap = sbblock->pool - sbblock->bitmap_size;

    for (u32 i = 0; i < count; ++i) {
        u32 cur = idx + i;
        u32 byte = cur / 8;
        u32 bit  = cur % 8;
        bitmap[byte] |= (1 << bit);
    }

    return idx;
}


static inline void shd_sbblock_free_st(shd_sbblock_t *a, u32 idx) {
    if (!a || idx >= a->block_num) return;
    u8 *bitmap = a->pool - a->bitmap_size;
    u32 byte = idx / 8;
    u32 bit  = idx % 8;

    bitmap[byte] &= ~(1 << bit); // mark block free
}
static inline void shd_sbblock_free_run_st(shd_sbblock_t *a, u32 idx, u32 count) {
    if (!a || idx + count > a->block_num) return;
    u8 *bitmap = a->pool - a->bitmap_size;

    for (u32 i = 0; i < count; ++i) {
        u32 cur = idx + i;
        u32 byte = cur / 8;
        u32 bit  = cur % 8;

        bitmap[byte] &= ~(1 << bit);
    }
}

static inline void *shd_sbblock_get_mt(shd_sbblock_t *sbblock, u32 idx) {
    if(idx >= sbblock->block_num) return 0;
    return sbblock->pool + idx * sbblock->block_size;
}


#endif /* __SHD_BLOCK_H__ */