#ifndef __SHD_STACK_H__
#define __SHD_STACK_H__

#include "../num.h"

#define SHD_ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define SHD_MEMDFFLAGS  0x80000000u
#define SHD_MEMDFVALUE 0x7FFFFFFFu

struct shd_dfmeta {
    u32 lcount; // distance in bytes to previous meta, flag collapse
    u32 ncount; // distance in bytes to next meta
};

typedef struct shd_dfstack {
    u8 *pool; // start of memory
    u8 *last; // points to current top meta
    u32 size; // total pool size in bytes
} shd_dfstack_t;


static inline void shd_create_dfstack(shd_dfstack_t *vstack, void *pool, u32 size) {
    if(!vstack) return;

    vstack->pool = pool;
    vstack->last = vstack->pool;
    vstack->size = size;

    struct shd_dfmeta *meta = (struct shd_dfmeta *)vstack->last;
    meta->lcount = 0;
    meta->ncount = 0;
}
static inline void shd_destroy_dfstack(shd_dfstack_t *vstack) {
    if(!vstack) return;

    vstack->pool = 0;
    vstack->last = 0;
    vstack->size = 0;
}

static inline void *shd_dfstack_push(shd_dfstack_t *vstack, u32 size) {
    if (!vstack)
        return 0;

    // total distance between metas
    u32 span = sizeof(struct shd_dfmeta) + size;

    // bounds check
    if ((u32)(vstack->last - vstack->pool) + span > vstack->size)
        return 0;

    struct shd_dfmeta *head = (struct shd_dfmeta *)vstack->last;
    struct shd_dfmeta *foot = (struct shd_dfmeta *)(vstack->last + span);

    head->ncount = span;
    foot->lcount = span;
    foot->ncount = 0;

    vstack->last = (u8 *)foot;

    return (u8 *)head + sizeof(struct shd_dfmeta);
}
/* static inline void *shd_dfstack_push_aligned(
    shd_dfstack_t *vstack, u32 size, u32 align)
{
    if (!vstack || align == 0)
        return nullptr;

    u8 *payload_base = vstack->last + sizeof(struct shd_dfmeta);
    u32 pad = SHD_ALIGN_UP((uintptr_t)payload_base, align) -
              (uintptr_t)payload_base;

    return shd_dfstack_push(vstack, size + pad);
} */
static inline void shd_dfstack_pop(shd_dfstack_t *vstack, void *addr) {
    if (!vstack)
        return;

    if (vstack->last == vstack->pool)
        return;

    struct shd_dfmeta *foot = (struct shd_dfmeta *)vstack->last;
    struct shd_dfmeta *head =
        (struct shd_dfmeta *)((u8 *)foot -
            (foot->lcount & SHD_MEMDFVALUE));

    // address-based pop
    if (addr) {
        u8 *p = (u8 *)addr;

        if (p <= vstack->pool + sizeof(struct shd_dfmeta) ||
            p >= vstack->last)
            return;

        head = (struct shd_dfmeta *)(p - sizeof(struct shd_dfmeta));
        foot = (struct shd_dfmeta *)((u8 *)head + head->ncount);
    }

    // deferred free
    if (addr && (u8 *)foot != vstack->last) {
        foot->lcount |= SHD_MEMDFFLAGS;
        return;
    }

    // immediate collapse
    vstack->last = (u8 *)head;

    // cascade deferred collapses
    while (vstack->last != vstack->pool) {
        struct shd_dfmeta *prev =
            (struct shd_dfmeta *)vstack->last;

        if ((prev->lcount & SHD_MEMDFFLAGS) == 0)
            break;

        vstack->last -= (prev->lcount & SHD_MEMDFVALUE);
    }
}
static inline void shd_dfstack_clear(shd_dfstack_t *vstack) {
    if (!vstack)
        return;

    vstack->last = vstack->pool;

    struct shd_dfmeta *meta = (struct shd_dfmeta *)vstack->pool;
    meta->lcount = 0;
    meta->ncount = 0;
}


/* #region Sanity checks # */
/* ####################### */
_Static_assert(sizeof(struct shd_dfmeta) == 8, "shd_dfmeta must be 64 bits");
/* #endregion            # */
/* ####################### */

#endif /* __SHD_STACK_H__ */