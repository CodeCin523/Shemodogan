#include <shemodogan/mem/shd_block.h>
#include <shemodogan/mem/shd_mem.h>
#include <shemodogan/shd_core.h>

#include <stdlib.h>
#include <string.h>


struct shd_dataact {
    shd_sbblock_t       instdt_alloc;
    shd_actor_meta_t   *meta;
};
struct shd_datahnd {
    shd_handler_meta_t *meta;
    // shd_basehnd_t      *instdt;
    u32                 reg_actcount,
                        reg_hndcount;
    shd_hndid_t         id;
    u8                  status;
};


static struct shd_dataact *dataact;         // all registered actors
static u32 dataact_count = 0, dataact_size = 0;
static struct shd_datahnd *datahnd;         // all registered handler
static u32 datahnd_count = 0, datahnd_size = 0;

static shd_sbblock_t actpool_alloc = {0};   // main allocator for all instdt_alloc of the actors.


/* ================================================================================ */
/*  CORE                                                                            */
/* ================================================================================ */

// Binary Search
// Find Handler Current Index
static inline u32 find_hci(shd_hndid_t hndid) {
    u32 low = 0, high = datahnd_count, mid;

    while(low < high) {
        mid = low + (high - low) / 2;
        if(datahnd[mid].id == hndid)
            return mid;
        if(datahnd[mid].id < hndid)
            low = mid + 1;
        else
            high = mid;
    }

    return u32_MAX;
}
// Find Handler Insert Index
static inline u32 find_hii(shd_hndid_t hndid) {
    u32 low = 0, high = datahnd_count, mid;

    while(low < high) {
        mid = low + (high - low) / 2;
        // if(items[mid].id == hnd_id)
        //     return mid;
        if(datahnd[mid].id < hndid)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}


// Library Life-Cycle and Registers
shd_result_t shd_init_st(shd_crtflags_t flags) {
    // Actor & Handler arrays
    dataact = calloc(SHD_KB(2), sizeof(struct shd_dataact));
    if(!dataact) return SHD_RESULT_EXTERNAL_ALLOC;

    dataact_size = SHD_KB(2);
    dataact_count = 0;

    datahnd = calloc(256, sizeof(struct shd_datahnd));
    if(!datahnd) return SHD_RESULT_EXTERNAL_ALLOC;

    datahnd_size = 256;
    datahnd_count = 0;

    // Block & Stack allocs
    void *pool = malloc(SHD_MB(1));
    if(!pool) return SHD_RESULT_EXTERNAL_ALLOC;
    shd_setup_sbblock_st(&actpool_alloc, pool, SHD_MB(1), SHD_KB(1));

    // Auto Register
    /* if( !(flags & SHD_CRTFLAGS_AUTO_REGISTER) ) {
        shd_result_t res = SHD_RESULT_OK;

        res = shd_register_handler_st(SHD_HND_LOGGER_ID, );
        if(res) return res;

        res = shd_register_handler_st(SHD_HND_VLKCORE_ID, );
        if(res) return res;
        res = shd_register_handler_st(SHD_HND_VLKSEQUENCER_ID, );
        if(res) return res;
    } */

    return SHD_RESULT_OK;
}
shd_result_t shd_term_st(void) {
    // Should check current status of handlers and actors

    if(actpool_alloc.pool) {
        free(actpool_alloc.pool);
        shd_teardown_sbblock_st(&actpool_alloc);
    }
    if(datahnd) {
        free(datahnd);
        datahnd = 0;
        datahnd_size = 0;
        datahnd_count = 0;
    }
    if(dataact) {
        free(dataact);
        dataact = 0;
        dataact_size = 0;
        dataact_count = 0;
    }

    return SHD_RESULT_OK;
}

shd_result_t shd_register_actor_st(shd_actid_t actid, shd_actor_meta_t *meta) {

}
shd_result_t shd_register_handler_st(shd_hndid_t hndid, shd_handler_meta_t *meta) {
    // Safety checks
    if(!meta || !meta->fInit || !meta->fTerm || !meta->pName)
        return SHD_RESULT_INVALID_ARGUMENTS;
    if(datahnd_count+1 >= datahnd_size)
        return SHD_RESULT_EXTERNAL_ALLOC;

    if(find_hci(hndid) != u32_MAX)
        return SHD_RESULT_ID_EXISTS;

    // If no instance given, reserve space for it (not implemented yet...)
    if(!meta->pInst)
        return SHD_RESULT_INTERNAL_ALLOC;

    // Place it in the array
    u32 pos = find_hii(hndid);
    if(pos != datahnd_count) {
        memmove(&datahnd[pos + 1],
                &datahnd[pos],
                (datahnd_count - pos) * sizeof(struct shd_datahnd));
    }
    datahnd[pos].meta = meta;
    // datahnd[pos].instdt = 
    datahnd[pos].id = hndid;

    ++datahnd_count;
    return SHD_RESULT_OK;
}

// Handler Life-Cycle

shd_result_t shd_handler_init_st(shd_hndid_t hndid, shd_basecrt_t *creator) {
    u32 pos = find_hci(hndid);
    if(pos == u32_MAX)
        return SHD_RESULT_ID_NOT_FOUND;

    struct shd_datahnd *data = &datahnd[pos];
    shd_handler_meta_t *meta = data->meta;

    { // Check dependencies
        for(u16 i = 0; i < meta->depCount; i++) {
            u32 j = find_hci(meta->pDeps[i]);

            if(j == u32_MAX || datahnd[j].status == 0)
                return SHD_RESULT_MISSING_DEPENDENCIES;
        }
    }

    shd_result_t res = meta->fInit(creator);

    if(res == SHD_RESULT_OK) {
        for(u16 i = 0; i < meta->depCount; i++) {
            u32 j = find_hci(meta->pDeps[i]);

            ++datahnd[j].reg_hndcount;
        }
        data->status = 1;
    }
    return res;
}
shd_result_t shd_handler_term_st(shd_hndid_t hndid) {
    u32 pos = find_hci(hndid);
    if(pos == u32_MAX)
        return SHD_RESULT_ID_NOT_FOUND;

    struct shd_datahnd *data = &datahnd[pos];
    shd_handler_meta_t *meta = data->meta;

    { // Check dependents
        if(data->reg_actcount == 0) return SHD_RESULT_EXISTENT_DEPENDENTS;
        if(data->reg_hndcount == 0) return SHD_RESULT_EXISTENT_DEPENDENTS;
    }

    shd_result_t res = meta->fTerm();

    if(res == SHD_RESULT_OK) {
        for(u16 i = 0; i < meta->depCount; i++) {
            u32 j = find_hci(meta->pDeps[i]);

            --datahnd[j].reg_hndcount;
        }
        data->status = 0;
    }
    return res;
}

shd_result_t shd_handler_get_mt(shd_hndid_t hndid, shd_basehnd_t *out_handler) {
    u32 pos = find_hci(hndid);
    if(pos == u32_MAX)
        return SHD_RESULT_ID_NOT_FOUND;

    out_handler = datahnd[pos].meta->pInst;
    return SHD_RESULT_OK;
}

// Actor Life-Cycle
shd_result_t shd_actor_subscribe_mt(shd_actid_t actid, shd_basecrt_t *creator, shd_baseact_t **out_actor) {

}
shd_result_t shd_actor_release_mt(shd_actid_t actid, shd_baseact_t *actor) {

}