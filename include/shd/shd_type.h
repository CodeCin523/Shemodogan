#ifndef __SHD_TYPE_H__
#define __SHD_TYPE_H__

#include "shd_id.h"
#include "shd_num.h"

typedef u32 shd_status_t;
enum {
    SHD_STATUS_SUCCESS = (SHD_ID32_GEN | 0),
    SHD_STATUS_FAILED  = (SHD_ID32_GEN | 1),
    SHD_STATUS_TOKEN_INVALID    = (SHD_ID32_TKN | 0),
    SHD_STATUS_HANDLER_INVALID_ID           = (SHD_ID32_HND | 0),
    SHD_STATUS_HANDLER_INVALID_CREATOR      = (SHD_ID32_HND | 1),
    SHD_STATUS_HANDLER_INVALID_DEPENDENCIES = (SHD_ID32_HND | 2),
    SHD_STATUS_HANDLER_INVALID_DEPENDENTS   = (SHD_ID32_HND | 3),
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

typedef struct shd_headcrt {
    shd_crttype_t  type;
    shd_crtflags_t flags;
} shd_headcrt_t;
typedef struct shd_headgtr {
    shd_gtrtype_t  type;
    shd_gtrflags_t flags;
} shd_headgtr_t;
typedef struct shd_headhnd {

} shd_headhnd_t;

typedef u64 shd_token_t;

typedef struct shd_actor {
    shd_token_t tkn;
} shd_actor_t;

typedef struct shd_handler {
    // Handler function pointers.
    shd_status_t (*fInitialize)(shd_headcrt_t *);
    shd_status_t (*fTerminate)();

    shd_headhnd_t *(*fGet)(shd_headgtr_t *);

    const char *pName;          // Name of handler.
    const shd_hid16_t *pDeps;   // Dependencies of handler.
    const shd_hid16_t depCount; // Count of dependencies.
    const shd_hid16_t len;      // Size of handler's struct.
    const shd_hid16_t id;       // Id of handler.
} shd_handler_t;

#endif /* __SHD_TYPE_H__ */