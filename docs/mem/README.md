# Memory Utilities (`mem`)

Custom memory utilities for the Shemodogan runtime.

⚠ These utilities manipulate raw memory and alignment manually. Improper usage may break library invariants or cause crashes. All memory alignment is **the responsibility of the caller**, not the utility implementation.

## Files

| File | Description |
|------|-------------|
| [shd_stack.h](shd_stack.md) | Low-level deferred stack implementation. Unsafe: for internal library usage only. |