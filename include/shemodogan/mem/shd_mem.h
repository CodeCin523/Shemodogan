#ifndef __SHD_MEM_H__
#define __SHD_MEM_H__

#include "../shd_num.h"


#define SHD_ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define SHD_KB(num) (num * 1024)
#define SHD_MG(num) (num * 1048576)
#define SHD_GB(num) (num * 1073741824)


/* ================================================================================ */
/*  INTERFACE                                                                       */
/* ================================================================================ */

// Align 'x' up to the next multiple of 'a' (a must be power of 2)
static inline u64 shd_align_up(u64 x, u64 a) {
    return (x + (a - 1)) & ~(a - 1);
}

// Calculate the padding needed to align a pointer to 'alignment' (power of 2)
static inline u64 shd_padding(void *ptr, u64 alignment) {
    uintptr_t addr = (uintptr_t)ptr;
    return (alignment - (addr % alignment)) % alignment;
}


#endif /* __SHD_MEM_H__ */