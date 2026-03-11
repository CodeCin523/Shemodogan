#ifndef __SHD_CORE_H__
#define __SHD_CORE_H__

#include "shd_num.h"


/* ================================================================================ */
/*  API RULES                                                                       */
/* ================================================================================ */

/* module abbreviations (optional, when useful)
 * ├─ crt                       : creator
 * ├─ gtr                       : getter
 * ├─ tkn                       : token
 * ├─ evt                       : event
 * ├─ act                       : actor
 * └─ hnd                       : handler
 *
 * naming convention
 * ├─ case
 * |  ├─ snake_case             : functions, types, variables
 * |  └─ SCREAMING_SNAKE_CASE   : constants, macros, enum values
 * ├─ prefix
 * |  └─ shd_                   : yes.
 * ├─ function suffix
 * |  ├─ st                     : single-thread only
 * |  ├─ mt                     : multi-thread safe
 * |  └─ {st,mt}[0-9]*          : version of the function
 * └─ type suffix
 *    └─ _t                     : yes.
*/

#define SHD_PROVIDER_STD (0)
#define SHD_PROVIDER_EXT (1)


/* ================================================================================ */
/*  TYPEDEF                                                                         */
/* ================================================================================ */

#include "utils/shd_idutils.h"

typedef u32 shd_result_t;
enum {
    SHD_RESULT_OK                       = 0,
    SHD_RESULT_FAILED                   = 1,
    SHD_RESULT_FAILED_INTERNAL_ALLOC    = 2,
    SHD_RESULT_FAILED_EXTERNAL_ALLOC    = 3,
};

typedef u16 shd_hndid_t;
typedef u32 shd_actid_t;
enum {
    // Handler-Id
    SHD_HND_LOGGER_ID       = SHD_IDUTILS_MAKE_HNDID(SHD_PROVIDER_STD,  0),
    SHD_HND_VLKCORE_ID      = SHD_IDUTILS_MAKE_HNDID(SHD_PROVIDER_STD,  8),
    SHD_HND_VLKSWAPCHAIN_ID = SHD_IDUTILS_MAKE_HNDID(SHD_PROVIDER_STD,  9),
    SHD_HND_VLKSEQUENCER_ID = SHD_IDUTILS_MAKE_HNDID(SHD_PROVIDER_STD, 10),

    // Actor-Id
    SHD_ACT_VLKDEVICE_ID    = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKCORE_ID, SHD_IDUTILS_TYPE_ACT, 0),
};

typedef u32 shd_crttype_t;
typedef u32 shd_gtrtype_t;
enum {
    // Creator Type
    SHD_CRTTYPE_LOGGER          = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_LOGGER_ID, SHD_IDUTILS_TYPE_CRT, 0),
    SHD_CRTTYPE_VLKCORE         = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKCORE_ID, SHD_IDUTILS_TYPE_CRT, 0),
    SHD_CRTTYPE_VLKSWAPCHAIN    = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKSWAPCHAIN_ID, SHD_IDUTILS_TYPE_CRT, 0),
    SHD_CRTTYPE_VLKSEQUENCER    = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKSEQUENCER_ID, SHD_IDUTILS_TYPE_CRT, 0),

    // Getter Type
    SHD_GTRTYPE_LOGGER          = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_LOGGER_ID, SHD_IDUTILS_TYPE_GTR, 0),
    SHD_GTRTYPE_VLKCORE         = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKCORE_ID, SHD_IDUTILS_TYPE_GTR, 0),
    SHD_GTRTYPE_VLKSWAPCHAIN    = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKSWAPCHAIN_ID, SHD_IDUTILS_TYPE_GTR, 0),
    SHD_GTRTYPE_VLKSEQUENCER    = SHD_IDUTILS_MAKE_SUBID(
        SHD_HND_VLKSEQUENCER_ID, SHD_IDUTILS_TYPE_GTR, 0),
};

typedef u32 shd_crtflags_t;
typedef u32 shd_gtrflags_t;
enum {
    // Creator Flags
    SHD_CRTFLAGS_DEFAULT_CREATOR = (1 << 0),

    // Getter Flags
    SHD_GTRFLAGS_DIRECT_INSTANCE = (1 << 0),
};


typedef union {
    struct {
        shd_crttype_t ctype;
        shd_crtflags_t flags;
    };
    u64 _raw;
} shd_basecrt_t;
typedef union {
    struct {
        shd_gtrtype_t gtype;
        shd_gtrflags_t flags;
    };
    u64 _raw;
} shd_basegtr_t;

typedef union {
    struct {

    };
    u64 _raw;
} shd_baseact_t;
typedef union {
    struct {

    };
    u64 _raw;
} shd_basehnd_t;

typedef struct shd_actor_meta {
    shd_result_t (*fInit)(shd_basecrt_t *);
    shd_result_t (*fTerm)();

    shd_baseact_t *(*fGet)(shd_basegtr_t *);

    const shd_hndid_t parentHandler;
    const u16 datalen;          // Size of actors's struct.

} shd_actor_meta_t;
typedef struct shd_handler_meta {
    shd_result_t (*fInit)(shd_basecrt_t *);
    shd_result_t (*fTerm)();

    shd_basehnd_t *(*fGet)(shd_basegtr_t *);

    const char *pName;          // Name of handler.
    const shd_hndid_t *pDeps;   // Dependencies of handler.
    const u16 depCount;         // Count of dependencies.
    const u16 datalen;          // Size of handler's struct.
} shd_handler_meta_t;

#define SHD_UTILS_UNDEF
#include "utils/shd_idutils.h"
#undef SHD_UTILS_UNDEF


/* ================================================================================ */
/*  INTERFACE                                                                       */
/* ================================================================================ */

// Library Life-Cycle and Registers
shd_result_t shd_init_st();
shd_result_t shd_term_st();

shd_result_t shd_register_actor_st();
shd_result_t shd_register_handler_st();

// Handler Life-Cycle
shd_result_t shd_handler_init_st();
shd_result_t shd_handler_term_st();

shd_result_t shd_handler_get_mt();

// Actor Life-Cycle
shd_result_t shd_actor_subscribe_mt();
shd_result_t shd_actor_release_mt();

shd_result_t shd_actor_get_mt();


/* ================================================================================ */
/*  INTERFACE ALIAS                                                                 */
/* ================================================================================ */

#define shd_init(...) shd_init_st(__VA_ARGS__)
#define shd_term(...) shd_term_st(__VA_ARGS__)

#define shd_register_actor(...) shd_register_actor_st(__VA_ARGS__)
#define shd_register_handler(...) shd_register_handler_st(__VA_ARGS__)

#define shd_handler_init(...) shd_handler_init_st(__VA_ARGS__)
#define shd_handler_term(...) shd_handler_term_st(__VA_ARGS__)

#define shd_handler_get(...) shd_handler_get_mt(__VA_ARGS__)

#define shd_actor_subscribe(...) shd_actor_subscribe_mt(__VA_ARGS__)
#define shd_actor_release(...) shd_actor_release_mt(__VA_ARGS__)

#define shd_actor_get(...) shd_actor_get_mt(__VA_ARGS__)


#endif /* __SHD_CORE_H__ */