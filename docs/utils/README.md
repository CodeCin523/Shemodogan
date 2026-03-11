# Utils

Low-level helper utilities for the Shemodogan runtime.

⚠ These utilities manipulate internal components and are **not intended for general use** unless the caller fully understands the underlying system. Incorrect usage may produce invalid runtime states.

All files in this subdirectory are **macro-heavy**. Each provides a mechanism (`SHD_UTILS_UNDEF`) to undefine all macros it declares, which is intended purely for **namespace cleanliness**. Using or omitting this mechanism incorrectly may lead to **unpredictable behavior** in your code, so it should be applied with care.

## Files

| File | Description |
|------|-------------|
| [shd_idutils.h](shd_idutils.md) | Utilities for constructing and inspecting handler and sub identifiers. Includes macros for creating, masking, and extracting HNDIDs and SUBIDs. ⚠ Macro-heavy, supports `SHD_UTILS_UNDEF` to clean up definitions. |