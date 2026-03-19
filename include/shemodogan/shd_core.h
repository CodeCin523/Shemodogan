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
 *
 * functions general standard
 * ├─ return shd_result_t
 * └─ output values use last out_* parameter
*/

#define SHD_PROVIDER_STD (0)
#define SHD_PROVIDER_EXT (1)


/* ================================================================================ */
/*  TYPEDEF                                                                         */
/* ================================================================================ */

#include "utils/shd_idutils.h"

typedef u32 shd_result_t;
enum {
    // General results
    SHD_RESULT_OK                         = 0,   // Success
    SHD_RESULT_FAILED                     = 1,   // Generic failure

    // Argument / input errors (10–19)
    SHD_RESULT_INVALID_ARGUMENTS          = 10,  // One or more arguments invalid
    SHD_RESULT_MISSING_OUTPUT             = 11,  // Output pointer required but NULL

    // Memory allocation errors (20–29)
    SHD_RESULT_INTERNAL_ALLOC             = 20,  // Allocation failed internally
    SHD_RESULT_EXTERNAL_ALLOC             = 21,  // Allocation failed from external source

    // ID / entity errors (30–39)
    SHD_RESULT_ID_EXISTS                  = 30,  // Trying to create an entity with an existing ID
    SHD_RESULT_ID_NOT_FOUND               = 31,  // Entity with the given ID does not exist

    // Dependency errors (40–49)
    SHD_RESULT_MISSING_DEPENDENCIES       = 40,  // Required dependencies are missing
    SHD_RESULT_EXISTENT_DEPENDENTS        = 41   // Cannot delete due to existing dependents
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
typedef u32 shd_crtflags_t;
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

    // Creator Flags
    SHD_CRTFLAGS_DEFAULT_CREATOR = (1 << 0),
    SHD_CRTFLAGS_AUTO_REGISTER   = (1 << 1)
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

    const u16 datalen;          // Size of actors's struct.

} shd_actor_meta_t;
typedef struct shd_handler_meta {
    shd_result_t (*fInit)(shd_basecrt_t *);
    shd_result_t (*fTerm)();

    shd_basehnd_t *pInst;       // the handler struct. if null, the core creates it.

    const char *pName;          // Name of handler.
    const shd_hndid_t *pDeps;   // Dependencies of handler.
    const u16 depCount;         // Count of dependencies.
    const u16 instlen;          // Size of handler's struct.
} shd_handler_meta_t;

#define SHD_UTILS_UNDEF
#include "utils/shd_idutils.h"
#undef SHD_UTILS_UNDEF


/* ================================================================================ */
/*  INTERFACE                                                                       */
/* ================================================================================ */

// Library Life-Cycle and Registers
shd_result_t shd_init_st(shd_crtflags_t flags);
shd_result_t shd_term_st(void);

shd_result_t shd_register_actor_st(shd_actid_t actid, shd_actor_meta_t *meta);
shd_result_t shd_register_handler_st(shd_hndid_t hndid, shd_handler_meta_t *meta);

// Handler Life-Cycle
shd_result_t shd_handler_init_st(shd_hndid_t hndid, shd_basecrt_t *creator);
shd_result_t shd_handler_term_st(shd_hndid_t hndid);

shd_result_t shd_handler_get_mt(shd_hndid_t hndid, shd_basehnd_t *out_handler);

// Actor Life-Cycle
shd_result_t shd_actor_subscribe_mt(shd_actid_t actid, shd_basecrt_t *creator, shd_baseact_t **out_actor);
shd_result_t shd_actor_release_mt(shd_actid_t actid, shd_baseact_t *actor);


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


#endif /* __SHD_CORE_H__ */