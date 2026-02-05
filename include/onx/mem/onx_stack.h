#ifndef __ONX_STACK_H__
#define __ONX_STACK_H__

#include "../onx_num.h"

#define ONX_MEMDFFLAGS 0b10000000'00000000'00000000'00000000
#define ONX_MEMDFVALUE 0b01111111'11111111'11111111'11111111

struct onx_dfmeta {
    u32 lcount; // last count, flag collapse
    u32 ncount; // next count
};

typedef struct onx_dfstack {
    u64 *pool;
    struct onx_dfmeta *last;
    u32 count;
} onx_dfstack_t;

static inline u64 *onx_dfstack_push(onx_dfstack_t *vstack, u32 size) {
    if(vstack == nullptr)
        return nullptr;

    // The len logics works, but if size is a multiple of u64, it gives 8 bytes (64bits) bonus.
    u32 len = (size / sizeof(u64)) + 2;
    u32 used = vstack->last - (struct onx_dfmeta *)vstack->pool;
    if(len >= vstack->count - used)
        return nullptr;

    struct onx_dfmeta *head = vstack->last;
    struct onx_dfmeta *foot = vstack->last + len;

    head->ncount = len;
    
    foot->lcount = len;
    foot->ncount = 0;

    vstack->last = foot;
    return (u64*)head + 1;
}
static inline void onx_dfstack_pop(onx_dfstack_t *vstack, u64 *addr) {
    if(vstack == nullptr)
        return;
    if(vstack->pool == (u64 *)vstack->last /*|| vstack->last->lcount == 0*/)
        return;
    
    struct onx_dfmeta *head = vstack->last - (vstack->last->lcount & ONX_MEMDFVALUE);
    if(addr != nullptr) {
        if (addr < vstack->pool + 1 || addr >= (u64 *)vstack->last)
            return;
        head = (struct onx_dfmeta *) (addr - 1);
    }
    struct onx_dfmeta *foot = head + head->ncount;

    if(addr != nullptr && foot != vstack->last) {
        foot->lcount |= ONX_MEMDFFLAGS;
    } else {
        // Really only there for sanity, since they will get overwritten if pushed on.
        //  foot->lcount = 0;
        //  foot->flags = 0;
        //  head->ncount = 0;
        vstack->last = head;
        while ((u64 *)vstack->last != vstack->pool) {
            // if current last doesn't need to be removed
            if ((vstack->last->lcount & ONX_MEMDFFLAGS) == 0)
                break;
            // otherwise change the last
            vstack->last = vstack->last - (vstack->last->lcount & ONX_MEMDFVALUE);
        }
    }
}
static inline void onx_dfstack_clear(onx_dfstack_t *vstack) {
    vstack->last = (struct onx_dfmeta *) vstack->pool;
    vstack->last->lcount = 0;
    vstack->last->ncount = 0;
}

#endif /* __ONX_STACK_H__ */