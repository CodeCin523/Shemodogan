#include <shd/mem/shd_stack.h>

#include <stdlib.h>
#include <stdio.h>

static void dump_stkspace(
    const struct shd_dfmeta *s,
    const struct shd_dfmeta *last,
    u32 idx
) {
    printf(
        "  [%u] %p | flags=%u lastc=%u nextc=%u%s\n",
        idx,
        (void *)s,
        (s->lcount & SHD_MEMDFFLAGS) != 0,
        s->lcount & SHD_MEMDFVALUE,
        s->ncount,
        (s == last) ? "  <== LAST" : ""
    );

    if ((s->lcount & SHD_MEMDFFLAGS) != 0) {
        printf("       -> unordered-pop (cascade pending)\n");
    }
}

static void dump_all(
    const char *title,
    struct shd_dfmeta **spaces,
    u32 count,
    const struct shd_dfmeta *last
) {
    printf("\n==== %s ====\n", title);
    for (u32 i = 0; i < count; ++i) {
        dump_stkspace(spaces[i], last, i);
    }
}

static u32 test_vstack(shd_dfstack_t *vstk) {
    void *bufs[8] = {0};
    struct shd_dfmeta *spaces[8];
    u32 scount = 0;
    u32 bcount = 0;

    /* Head sentinel meta */
    spaces[scount++] = (struct shd_dfmeta *)vstk->pool;

#define PUSH(sz)                                                        \
    do {                                                                \
        printf("\n-- PUSH %u x u64 --\n", (sz));                        \
        bufs[bcount] = shd_dfstack_push(vstk, (sz) * sizeof(u64));     \
        if (!bufs[bcount]) {                                            \
            printf("push failed (%u)\n", (sz));                         \
            return 1;                                                   \
        }                                                               \
        spaces[scount++] = (struct shd_dfmeta *)vstk->last;             \
        bcount++;                                                       \
        dump_all("after push", spaces, scount,                           \
                 (struct shd_dfmeta *)vstk->last);                      \
    } while (0)

    /* Initial allocations */
    PUSH(2); /* bufs[0] */
    PUSH(3); /* bufs[1] */
    PUSH(1); /* bufs[2] */
    PUSH(4); /* bufs[3] */

    /* 1. Pop alloc of size 3 (unordered) */
    printf("\n-- POP size-3 allocation (unordered) --\n");
    shd_dfstack_pop(vstk, bufs[1]);
    dump_all("after unordered pop (size 3)",
             spaces, scount,
             (struct shd_dfmeta *)vstk->last);

    /* 2. Pop last allocation by address (size 4) */
    printf("\n-- POP last allocation by address (size 4) --\n");
    shd_dfstack_pop(vstk, bufs[3]);
    dump_all("after pop last by address",
             spaces, scount,
             (struct shd_dfmeta *)vstk->last);

    /* 3. Re-add size 4 */
    printf("\n-- RE-PUSH 4 x u64 --\n");
    bufs[3] = shd_dfstack_push(vstk, 4 * sizeof(u64));
    spaces[scount++] = (struct shd_dfmeta *)vstk->last;
    dump_all("after re-push size 4",
             spaces, scount,
             (struct shd_dfmeta *)vstk->last);

    /* 4. Pop everything normally */
    printf("\n-- NORMAL POP (LIFO) UNTIL EMPTY --\n");
    while (vstk->last != vstk->pool) {
        shd_dfstack_pop(vstk, NULL);
        dump_all("after normal pop",
                 spaces, scount,
                 (struct shd_dfmeta *)vstk->last);
    }

#undef PUSH

    return 0;
}

#ifndef TEST_NOMAIN
int main(void) {
    shd_dfstack_t vstk = {0};

    const u32 POOL_BYTES = 32 * sizeof(u64);

    vstk.pool = calloc(1, POOL_BYTES);
    if (!vstk.pool) {
        printf("Failed to reserve memory\n");
        return 1;
    }

    vstk.last = vstk.pool;
    vstk.size = POOL_BYTES;

    if (test_vstack(&vstk)) {
        free(vstk.pool);
        return 1;
    }

    free(vstk.pool);
    printf("\n==== FULL SUCCESS ====\n");
    return 0;
}
#endif
