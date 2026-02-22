/* SHD Naming Rules - Simplified
 * ├─ CMake modules
 * |  ├─ shdl => SHD Library (main target)
 * |  ├─ shdt => SHD Tests
 * |  └─ shde => SHD Examples
 * ├─ Module abbreviations (optional, when useful)
 * |  ├─ gen  => general
 * |  ├─ tkn  => token
 * |  ├─ evt  => event
 * |  ├─ act  => actor
 * |  └─ hnd  => handler
 * ├─ Other abbreviations
 * |  ├─ crt => creator
 * |  ├─ gtr => getter
 * |
 * ├─ Case
 * |  ├─ snake_case           => functions, types, variables
 * |  └─ SCREAMING_SNAKE_CASE => only constants, macros, enum values
 * ├─ Prefix
 * |  ├─ shd_  => functions, types (struct, enum, typedef), variables
 * |  └─ SHD_  => constants, macros, enum values
 * |
 * ├─ Function
 * |  └─ shd_do_something()
 * ├─ Type (struct / enum / typedef)
 * |  └─ shd_something_t
 */

#ifndef __SHD_H__
#define __SHD_H__

#include "type.h"

// Library lifecycle
shd_status_t shd_init();
shd_status_t shd_exit();

// Handler registration
shd_status_t shd_register_handler(shd_hid16_t hnd_id, shd_handler_meta_t *hnd);

// Dependency checks (optional, for introspection)
shd_status_t shd_check_dependencies(shd_hid16_t hnd_id, shd_handler_meta_t *hnd);
shd_status_t shd_check_dependents(shd_hid16_t hnd_id, shd_handler_meta_t *hnd);

// Handler runtime interface
shd_status_t shd_handler_initialize(shd_hid16_t hnd_id, shd_basecrt_t *creator);
shd_status_t shd_handler_terminate(shd_hid16_t hnd_id);
shd_basehnd_t *shd_handler_get(shd_hid16_t hnd_id, shd_basegtr_t *getter);

#endif /* __SHD_H__ */