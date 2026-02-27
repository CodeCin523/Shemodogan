#ifndef __SHD_TYPE_H__
#define __SHD_TYPE_H__

#include "num.h"


/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # Id                                          # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

typedef u16 shd_id16_t;
enum {
    SHD_ID16_GEN = 0b0000'0000'00000000,
    SHD_ID16_TKN = 0b1000'0000'00000000,
    SHD_ID16_EVT = 0b0100'0000'00000000,
    SHD_ID16_ACT = 0b1100'0000'00000000,
    SHD_ID16_HND = 0b0010'0000'00000000,
};
typedef u32 shd_id32_t;
enum {
    SHD_ID32_GEN = (SHD_ID16_GEN << 15),
    SHD_ID32_TKN = (SHD_ID16_TKN << 15),
    SHD_ID32_EVT = (SHD_ID16_EVT << 15),
    SHD_ID32_ACT = (SHD_ID16_ACT << 15),
    SHD_ID32_HND = (SHD_ID16_HND << 15),
};

typedef u16 shd_hid16_t;
enum {
    SHD_HID16_CORE             = (SHD_ID16_HND | 0b0000'00000000),
    SHD_HID16_LOGGER           = (SHD_ID16_HND | 0b0000'00000001),
    SHD_HID16_THREAD           = (SHD_ID16_HND | 0b0000'00000010),
    SHD_HID16_WINDOW           = (SHD_ID16_HND | 0b0000'00000011),
    SHD_HID16_VULKAN_INSTANCE  = (SHD_ID16_HND | 0b0001'00000000),
    SHD_HID16_VULKAN_DEVICES   = (SHD_ID16_HND | 0b0001'00000001),
    SHD_HID16_VULKAN_SWAPCHAIN = (SHD_ID16_HND | 0b0001'00000010),
};
typedef u32 shd_hid32_t;
enum {
    SHD_HID32_CORE             = (SHD_HID16_CORE               << 15),
    SHD_HID32_LOGGER           = (SHD_HID16_LOGGER             << 15),
    SHD_HID32_THREAD           = (SHD_HID16_THREAD             << 15),
    SHD_HID32_WINDOW           = (SHD_HID16_WINDOW             << 15),
    SHD_HID32_VULKAN_INSTANCE  = (SHD_HID16_VULKAN_INSTANCE    << 15),
    SHD_HID32_VULKAN_DEVICES   = (SHD_HID16_VULKAN_DEVICES     << 15),
    SHD_HID32_VULKAN_SWAPCHAIN = (SHD_HID16_VULKAN_SWAPCHAIN   << 15),
};


/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # Types                                       # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

typedef u32 shd_status_t;
enum {  // NEED TO FIND CORRECT VALUES FOR THOSE
    SHD_STATUS_SUCCESS                      = 0,
    SHD_STATUS_FAILED                       = 1,
    SHD_STATUS_FAILED_INTERN_ALLOC          = 2,
    SHD_STATUS_FAILED_EXTERN_ALLOC          = 3,
    SHD_STATUS_HANDLER_ID_EXISTS            = 4,
    SHD_STATUS_HANDLER_ID_NOT_FOUND         = 5,
    SHD_STATUS_HANDLER_INVALID_STATE        = 6,
    SHD_STATUS_HANDLER_MISSING_DEPENDENCIES = 7,
    SHD_STATUS_HANDLER_EXISTENT_DEPENDENTS  = 8,
    SHD_STATUS_HANDLER_MISSING_CREATOR      = 9,
    SHD_STATUS_HANDLER_INVALID_CREATOR      = 10,
    SHD_STATUS_HANDLER_INITIALIZED          = 100,
    SHD_STATUS_HANDLER_TERMINATED           = 101,
};

typedef u32 shd_crttype_t;
#define SHD_CRTTYPE_LOGGER          (SHD_HID32_LOGGER | 0)
#define SHD_CRTTYPE_VULKAN_INSTANCE (SHD_HID32_VULKAN_INSTANCE | 0)
typedef u32 shd_crtflags_t;
#define SHD_CRTFLAG_DEFAULT_CREATOR (1)

typedef u32 shd_gtrtype_t;
#define SHD_GTRTYPE_LOGGER          (SHD_HID32_LOGGER | 0)
#define SHD_GTRTYPE_VULKAN_INSTANCE (SHD_HID32_VULKAN_INSTANCE | 0)
typedef u32 shd_gtrflags_t;
#define SHD_GTRFLAG_DIRECT_INSTANCE (1)

typedef union {
    struct {
        shd_crttype_t type;
        shd_crtflags_t flags;
    };
    u64 _raw;
} shd_basecrt_t;
typedef union {
    struct {
        shd_gtrtype_t  type;
        shd_gtrflags_t flags;
    };
    u64 _raw;
} shd_basegtr_t;
typedef union {
    struct {

    };
    u64 _raw;
} shd_basehnd_t;

typedef u64 shd_token_t;

typedef struct shd_actor {
    shd_token_t tkn;
} shd_actor_t;

typedef struct shd_handler_meta {
    // Handler function pointers.
    shd_status_t (*fInitialize)(shd_basecrt_t *);
    shd_status_t (*fTerminate)();

    shd_basehnd_t *(*fGet)(shd_basegtr_t *);

    const char *pName;          // Name of handler.
    const shd_hid16_t *pDeps;   // Dependencies of handler.
    const u16 depCount;         // Count of dependencies.
    const u16 datalen;          // Size of handler's struct.
} shd_handler_meta_t;

#endif /* __SHD_TYPE_H__ */