#ifndef __SHD_STACK_H__
#define __SHD_STACK_H__

#include "../shd_num.h"

#define SHD_MEMDFFLAGS 0b10000000'00000000'00000000'00000000
#define SHD_MEMDFVALUE 0b01111111'11111111'11111111'11111111

struct shd_dfmeta {
    u32 lcount; // last count, flag collapse
    u32 ncount; // next count
};

typedef struct shd_dfstack {
    u64 *pool;
    struct shd_dfmeta *last;
    u32 count;
} shd_dfstack_t;

static inline u64 *shd_dfstack_push(shd_dfstack_t *vstack, u32 size) {
    if(vstack == nullptr)
        return nullptr;

    u32 len = (size - 7) / 8 + 2;
    u32 used = vstack->last - (struct shd_dfmeta *)vstack->pool;
    if(len >= vstack->count - used)
        return nullptr;

    struct shd_dfmeta *head = vstack->last;
    struct shd_dfmeta *foot = vstack->last + len;

    head->ncount = len;
    
    foot->lcount = len;
    foot->ncount = 0;

    vstack->last = foot;
    return (u64*)head + 1;
}
static inline void shd_dfstack_pop(shd_dfstack_t *vstack, u64 *addr) {
    if(vstack == nullptr)
        return;
    if(vstack->pool == (u64 *)vstack->last /*|| vstack->last->lcount == 0*/)
        return;
    
    struct shd_dfmeta *head = vstack->last - (vstack->last->lcount & SHD_MEMDFVALUE);
    if(addr != nullptr) {
        if (addr < vstack->pool + 1 || addr >= (u64 *)vstack->last)
            return;
        head = (struct shd_dfmeta *) (addr - 1);
    }
    struct shd_dfmeta *foot = head + head->ncount;

    if(addr != nullptr && foot != vstack->last) {
        foot->lcount |= SHD_MEMDFFLAGS;
    } else {
        // Really only there for sanity, since they will get overwritten if pushed on.
        //  foot->lcount = 0;
        //  foot->flags = 0;
        //  head->ncount = 0;
        vstack->last = head;
        while ((u64 *)vstack->last != vstack->pool) {
            // if current last doesn't need to be removed
            if ((vstack->last->lcount & SHD_MEMDFFLAGS) == 0)
                break;
            // otherwise change the last
            vstack->last = vstack->last - (vstack->last->lcount & SHD_MEMDFVALUE);
        }
    }
}
static inline void shd_dfstack_clear(shd_dfstack_t *vstack) {
    vstack->last = (struct shd_dfmeta *) vstack->pool;
    vstack->last->lcount = 0;
    vstack->last->ncount = 0;
}

#endif /* __SHD_STACK_H__ */