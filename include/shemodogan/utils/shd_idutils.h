#ifndef __SHD_IDUTILS_H__
#define __SHD_IDUTILS_H__

#include "../shd_num.h"

#endif /* __SHD_IDUTILS_H__ */


/* ================================================================================ */
/*  MACROS                                                                          */
/* ================================================================================ */

#define SHD_IDUTILS_MASK_PROVIDER   0b00111111
#define SHD_IDUTILS_MASK_INSTANCE   u8_MAX
#define SHD_IDUTILS_MASK_TYPE       0b00000011
#define SHD_IDUTILS_MASK_SUB        u16_MAX

#define SHD_IDUTILS_SHIFT_INSTANCE  0
#define SHD_IDUTILS_SHIFT_PROVIDER  8
#define SHD_IDUTILS_SHIFT_TYPE      16
#define SHD_IDUTILS_SHIFT_SUB       18

#define SHD_IDUTILS_TYPE_HND 0b00
#define SHD_IDUTILS_TYPE_GTR 0b01
#define SHD_IDUTILS_TYPE_CRT 0b10
#define SHD_IDUTILS_TYPE_ACT 0b11


/* pppppp_iiiiiiii */
#define SHD_IDUTILS_MAKE_HNDID(provider, instance) (u16)( \
      (((provider) & SHD_IDUTILS_MASK_PROVIDER) << SHD_IDUTILS_SHIFT_PROVIDER) \
    | (((instance) & SHD_IDUTILS_MASK_INSTANCE) << SHD_IDUTILS_SHIFT_INSTANCE) )


/* pppppp_iiiiiiii_tt_ssssssssssssss */
#define SHD_IDUTILS_MAKE_SUBID(hidid, type, sub) (u32)( \
      (((u32)(hidid)) << SHD_IDUTILS_SHIFT_TYPE) \
    | (((type) & SHD_IDUTILS_MASK_TYPE) << SHD_IDUTILS_SHIFT_TYPE) \
    | (((sub)  & SHD_IDUTILS_MASK_SUB)  << SHD_IDUTILS_SHIFT_SUB) )


#define SHD_IDUTILS_HHDID_PROVIDER(hidid) \
(u8)(((hidid) >> SHD_IDUTILS_SHIFT_PROVIDER) & SHD_IDUTILS_MASK_PROVIDER)

#define SHD_IDUTILS_HHDID_INSTANCE(hidid) \
(u8)(((hidid) >> SHD_IDUTILS_SHIFT_INSTANCE) & SHD_IDUTILS_MASK_INSTANCE)


#define SHD_IDUTILS_SUBID_TYPE(subid) \
(u8)(((subid) >> SHD_IDUTILS_SHIFT_TYPE) & SHD_IDUTILS_MASK_TYPE)

#define SHD_IDUTILS_SUBID_SUB(subid) \
(u16)(((subid) >> SHD_IDUTILS_SHIFT_SUB) & SHD_IDUTILS_MASK_SUB)


/* ================================================================================ */
/*  UTILS_UNDEF                                                                     */
/* ================================================================================ */

#ifdef SHD_UTILS_UNDEF

#undef SHD_IDUTILS_MASK_PROVIDER
#undef SHD_IDUTILS_MASK_INSTANCE
#undef SHD_IDUTILS_MASK_TYPE
#undef SHD_IDUTILS_MASK_SUB

#undef SHD_IDUTILS_SHIFT_INSTANCE
#undef SHD_IDUTILS_SHIFT_PROVIDER
#undef SHD_IDUTILS_SHIFT_TYPE
#undef SHD_IDUTILS_SHIFT_SUB

#undef SHD_IDUTILS_TYPE_GTR
#undef SHD_IDUTILS_TYPE_CRT
#undef SHD_IDUTILS_TYPE_ACT

#undef SHD_IDUTILS_MAKE_HNDID
#undef SHD_IDUTILS_MAKE_SUBID

#undef SHD_IDUTILS_HNDID_PROVIDER
#undef SHD_IDUTILS_HNDID_INSTANCE

#undef SHD_IDUTILS_SUBID_TYPE
#undef SHD_IDUTILS_SUBID_SUB

#endif