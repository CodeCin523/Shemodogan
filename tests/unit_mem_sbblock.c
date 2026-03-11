#include <stdio.h>
#include <string.h>

#include <shemodogan/mem/shd_block.h>

#define shd_setup_sbblock      shd_setup_sbblock_st
#define shd_teardown_sbblock   shd_teardown_sbblock_st
#define shd_sbblock_alloc      shd_sbblock_alloc_st
#define shd_sbblock_find_run   shd_sbblock_find_run_st
#define shd_sbblock_alloc_run  shd_sbblock_alloc_run_st
#define shd_sbblock_free       shd_sbblock_free_st
#define shd_sbblock_free_run   shd_sbblock_free_run_st
#define shd_sbblock_get        shd_sbblock_get_mt

#define POOL_SIZE 256
#define BLOCK_SIZE 8


static void print_bitmap(shd_sbblock_t *a)
{
    u8 *bitmap = a->pool - a->bitmap_size;

    printf("bitmap: ");

    for (u32 i = 0; i < a->bitmap_size; ++i) {
        for (u32 j = 0; j < 8; ++j) {
            printf("%u", (bitmap[i] >> j) & 1);
        }
        printf(" ");
    }

    printf("\n");
}


int main(void)
{
    printf("==== SBLOCK ALLOCATOR TEST ====\n\n");

    u8 pool[POOL_SIZE];
    memset(pool, 0, sizeof(pool));

    shd_sbblock_t alloc;

    printf("setup allocator\n");
    shd_setup_sbblock_st(&alloc, pool, POOL_SIZE, BLOCK_SIZE);
    print_bitmap(&alloc);
    printf("\n");


    /* ------------------------------------------ */
    printf("allocating 10 single blocks\n");

    u32 ids[32];

    for (int i = 0; i < 10; ++i) {
        ids[i] = shd_sbblock_alloc(&alloc);
        printf("alloc -> %u\n", ids[i]);
        print_bitmap(&alloc);
    }

    printf("\n");


    /* ------------------------------------------ */
    printf("free some blocks to create holes\n");

    shd_sbblock_free(&alloc, ids[2]);
    printf("free -> %u\n", ids[2]);
    print_bitmap(&alloc);

    shd_sbblock_free(&alloc, ids[5]);
    printf("free -> %u\n", ids[5]);
    print_bitmap(&alloc);

    shd_sbblock_free(&alloc, ids[7]);
    printf("free -> %u\n", ids[7]);
    print_bitmap(&alloc);

    printf("\n");


    /* ------------------------------------------ */
    printf("allocate 3 more singles (should reuse holes)\n");

    for (int i = 10; i < 13; ++i) {
        ids[i] = shd_sbblock_alloc(&alloc);
        printf("alloc -> %u\n", ids[i]);
        print_bitmap(&alloc);
    }

    printf("\n");


    /* ------------------------------------------ */
    printf("find and allocate run of 4\n");

    u32 run = shd_sbblock_find_run(&alloc, 4);
    printf("run start -> %u\n", run);

    if (run != u32_MAX) {
        shd_sbblock_alloc_run(&alloc, run, 4);
    }

    print_bitmap(&alloc);
    printf("\n");


    /* ------------------------------------------ */
    printf("allocate 5 more singles\n");

    for (int i = 13; i < 18; ++i) {
        ids[i] = shd_sbblock_alloc(&alloc);
        printf("alloc -> %u\n", ids[i]);
        print_bitmap(&alloc);
    }

    printf("\n");


    /* ------------------------------------------ */
    printf("free the run (count=4)\n");

    shd_sbblock_free_run(&alloc, run, 4);
    print_bitmap(&alloc);
    printf("\n");


    /* ------------------------------------------ */
    printf("allocate another run of 3\n");

    u32 run2 = shd_sbblock_find_run(&alloc, 3);
    printf("run start -> %u\n", run2);

    if (run2 != u32_MAX) {
        shd_sbblock_alloc_run(&alloc, run2, 3);
    }

    print_bitmap(&alloc);
    printf("\n");


    /* ------------------------------------------ */
    printf("free a few single blocks\n");

    shd_sbblock_free(&alloc, ids[0]);
    printf("free -> %u\n", ids[0]);
    print_bitmap(&alloc);

    shd_sbblock_free(&alloc, ids[9]);
    printf("free -> %u\n", ids[9]);
    print_bitmap(&alloc);

    shd_sbblock_free(&alloc, ids[12]);
    printf("free -> %u\n", ids[12]);
    print_bitmap(&alloc);

    printf("\n");


    /* ------------------------------------------ */
    printf("final allocations\n");

    for (int i = 18; i < 22; ++i) {
        ids[i] = shd_sbblock_alloc(&alloc);
        printf("alloc -> %u\n", ids[i]);
        print_bitmap(&alloc);
    }

    printf("\n");


    printf("teardown allocator\n");
    shd_teardown_sbblock_st(&alloc);

    printf("\n==== TEST COMPLETE ====\n");

    return 0;
}