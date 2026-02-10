#include "shd/shd_type.h"
#include <shd/shd.h>

#include <shd/mem/shd_stack.h>

#include <stdlib.h>
#include <string.h>


struct hnd_data {
    shd_handler_t *hnd;
    shd_headhnd_t *dt;
    shd_status_t status;
    shd_hid16_t hid;
    u16 childCount;
};


struct hnd_data *g_hnddts = 0;
u32 g_hnddtCount = 0, g_hnddtSize = 0;

shd_dfstack_t g_hndstatic_stack = {0};
shd_dfstack_t g_hndtemp_stack = {0};


/* #region Core # */
/* ############## */
shd_status_t shd_init() {
    // Handler array
    g_hnddts = calloc(128, sizeof(struct hnd_data));
    if(!g_hnddts)
        return SHD_STATUS_FAILED_EXTERN_ALLOC;

    g_hnddtSize = 128;
    g_hnddtCount = 0;
    
    // Stacks
    void *temp = calloc(1024 + 2, sizeof(u64));
    if(!temp)
        return SHD_STATUS_FAILED_EXTERN_ALLOC;
    
    shd_create_dfstack(&g_hndstatic_stack,
        temp, 4096);
    temp += 4096 + sizeof(u64);
    shd_create_dfstack(&g_hndtemp_stack,
        temp, 4096);

    return SHD_STATUS_SUCCESS;
}
shd_status_t shd_exit() {
    if(g_hndstatic_stack.pool) {
        free(g_hndstatic_stack.pool);
        shd_destroy_dfstack(&g_hndstatic_stack);
        shd_destroy_dfstack(&g_hndtemp_stack);
    }
    if(g_hnddts) {
        free(g_hnddts);
        g_hnddts = 0;
        g_hnddtSize = 0;
        g_hnddtCount = 0;
    }

    return SHD_STATUS_SUCCESS;
}
/* #endregion   # */
/* ############## */

/* #region Handler# */
/* ################ */

// Find Current Index
static inline u32 findci(shd_hid16_t hid) {
    u32 low = 0, high = g_hnddtCount, mid;

    while(low < high) {
        mid = low + (high - low) / 2;
        if(g_hnddts[mid].hid == hid)
            return mid;
        if(g_hnddts[mid].hid < hid)
            low = mid + 1;
        else
            high = mid;
    }

    return u32_MAX;
}
// Find Insert Index
static inline u16 findii(shd_hid16_t hid) {
    u32 low = 0, high = g_hnddtCount, mid;

    while(low < high) {
        mid = low + (high - low) / 2;
        // if(items[mid].id == hid)
        //     return mid;
        if(g_hnddts[mid].hid < hid)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}

shd_status_t shd_register_handler(shd_hid16_t hid, shd_handler_t *hnd) {
    if(findci(hid) != u32_MAX)
        return SHD_STATUS_HANDLER_ID_EXISTS;

    if(g_hnddtCount+1 >= g_hnddtSize)
        return SHD_STATUS_FAILED_EXTERN_ALLOC;

    shd_headhnd_t *dt = (shd_headhnd_t *) shd_dfstack_push(&g_hndstatic_stack, hnd->datalen + 8);
    if(dt == 0)
        return SHD_STATUS_FAILED_INTERN_ALLOC;

    u32 pos = findii(hid);
    if(pos != g_hnddtCount) {
        memmove(&g_hnddts[pos + 1],
                &g_hnddts[pos],
                g_hnddtCount - pos * sizeof(shd_handler_t));
    }
    g_hnddts[pos].hnd = hnd;
    g_hnddts[pos].dt = dt;
    g_hnddts[pos].status = SHD_STATUS_SUCCESS;
    g_hnddts[pos].hid = hid;
    g_hnddts[pos].childCount = 0;

    ++g_hnddtCount;

    return SHD_STATUS_SUCCESS;
}

shd_status_t shd_check_dependencies(shd_hid16_t hid, shd_handler_t *hnd) {
    u32 pos = findci(hid);
    if (pos == u32_MAX)
        return SHD_STATUS_HANDLER_ID_NOT_FOUND;

    for(u8 i = 0; i < g_hnddts[pos].hnd->depCount; ++i) {
        if(findci(g_hnddts[pos].hnd->pDeps[i]) == u32_MAX)
            return SHD_STATUS_FAILED;
    }
    return SHD_STATUS_SUCCESS;
}
shd_status_t shd_check_dependents(shd_hid16_t hid, shd_handler_t *hnd) {
    u32 pos = findci(hid);
    if(pos == u32_MAX)
        return SHD_STATUS_HANDLER_ID_NOT_FOUND;
    
    return g_hnddts[pos].childCount == 0? SHD_STATUS_SUCCESS : SHD_STATUS_FAILED;
}

shd_status_t shd_handler_initialize(shd_hid16_t hid, shd_headcrt_t *creator) {
    int pos = findci(hid);
    if(pos == u32_MAX)
        return SHD_STATUS_HANDLER_ID_NOT_FOUND;

    if(g_hnddts[pos].status == SHD_STATUS_HANDLER_INITIALIZED)
        return SHD_STATUS_HANDLER_INVALID_STATE;

    if(shd_check_dependencies(hid, g_hnddts[pos].hnd) != SHD_STATUS_SUCCESS)
        return SHD_STATUS_HANDLER_MISSING_DEPENDENCIES;

    shd_status_t status = g_hnddts[pos].hnd->fInitialize(creator);

    if(status == SHD_STATUS_SUCCESS) {
        for(u16 i = 0; i < g_hnddts[pos].hnd->depCount; ++i) {
            u32 j = findci(g_hnddts[pos].hnd->pDeps[i]);
            // should never happen, but just in case
            if(j == u32_MAX)
                continue;

            ++g_hnddts[j].childCount;
        }
        g_hnddts[pos].status = SHD_STATUS_HANDLER_INITIALIZED;
    } else
        g_hnddts[pos].status = SHD_STATUS_FAILED;
    return status;
}
shd_status_t shd_handler_terminate(shd_hid16_t hid) {
    int pos = findci(hid);
    if(pos == u32_MAX)
        return SHD_STATUS_HANDLER_ID_NOT_FOUND;

    if(g_hnddts[pos].status == SHD_STATUS_HANDLER_TERMINATED)
        return SHD_STATUS_HANDLER_INVALID_STATE;

    if(shd_check_dependents(hid, g_hnddts[pos].hnd) != SHD_STATUS_SUCCESS)
        return SHD_STATUS_HANDLER_EXISTENT_DEPENDENTS;

    shd_status_t status = g_hnddts[pos].hnd->fTerminate();

    if(status == SHD_STATUS_SUCCESS) {
        for(u16 i = 0; i < g_hnddts[pos].hnd->depCount; ++i) {
            u32 j = findci(g_hnddts[pos].hnd->pDeps[i]);
            // should never happen, but just in case
            if(j == u32_MAX)
                continue;

            --g_hnddts[j].childCount;
        }
        g_hnddts[pos].status = SHD_STATUS_HANDLER_TERMINATED;
    } else
        g_hnddts[pos].status = SHD_STATUS_HANDLER_INITIALIZED;
    return status;
}
shd_headhnd_t *shd_handler_get(shd_hid16_t hid, shd_headgtr_t *getter) {
    int pos = findci(hid);
    if(pos == u32_MAX)
        return 0;

    if(getter->flags & SHD_GTRFLAG_DIRECT_INSTANCE)
        return g_hnddts[pos].dt;
    else
        return g_hnddts[pos].hnd->fGet(getter);
}
/* #endregion     # */
/* ################ */