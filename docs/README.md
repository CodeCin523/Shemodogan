# Shemodogan

Main include folder for the Shemodogan runtime.

Contains the core API, basic numeric type definitions, and internal helper utilities used throughout the library.

⚠ Most files are low-level and provide type definitions or macros that underpin the runtime. Only use them if you understand the intended usage patterns.

## Subdirectories

| Directory | Description |
|-----------|-------------|
| [utils](utils/README.md) | Low-level helper utilities for constructing and inspecting internal runtime structures. Macro-heavy; includes a mechanism (`SHD_UTILS_UNDEF`) to remove all macros for namespace cleanliness. ⚠ Only use if you fully understand the system. |

## Files

| File | Description |
|------|-------------|
| [shd_num.h](shd_num.md)  | Defines fixed-width integer and floating-point types (i8, u16, f32, etc.) along with min/max constants. Ensures consistent behavior across platforms. |
| [shd_core.h](shd_core.md) | Core runtime API. Provides lifecycle control, handler registration, and actor access. |
