#include <shd/shd.h>

/* #region Core # */
/* ############## */
shd_status_t shd_init() {
    return SHD_STATUS_SUCCESS;
}
shd_status_t shd_exit() {
    return SHD_STATUS_SUCCESS;
}
/* #endregion   # */
/* ############## */

/* #region Handler# */
/* ################ */

// Find Current Index
/*
static inline u32 findci(shd_hid16_t hid) {
    u32 low = 0, high = itemCount, mid;

    while(low < high) {
        mid = low + (high - low) / 2;
        if(items[mid].id == hid)
            return mid;
        if(items[mid].id < hid)
            low = mid + 1;
        else
            high = mid;
    }

    return u32_MAX;
}
// Find Insert Index
static inline u16 findii(shd_hid16_t hid) {
    u32 low = 0, high = itemCount, mid;

    while(low < high) {
        mid = low + (high - low) / 2;
        // if(items[mid].id == hid)
        //     return mid;
        if(items[mid].id < hid)
            low = mid + 1;
        else
            high = mid;
    }
    return low;

}*/

shd_status_t shd_register_handler(shd_handler_t *hnd) {
    return SHD_STATUS_SUCCESS;
}

shd_status_t shd_check_dependencies(shd_handler_t *hnd) {

}
shd_status_t shd_check_dependents(shd_handler_t *hnd) {

}

shd_status_t shd_handler_initialize(shd_hid16_t hid, shd_headcrt_t *creator) {
    
}
shd_status_t shd_handler_terminate(shd_hid16_t hid) {

}
shd_headhnd_t *shd_handler_get(shd_hid16_t hid, shd_headgtr_t *getter) {

}
/* #endregion     # */
/* ################ */