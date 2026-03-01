#include <shd/num.h>

/* Test Implementation of Enums
 * This is a test file for the global enum and types of Shemodogan.
 * Every implementation have basic requirement :
 *  - it cannot be type-safe, as every extensions need to be able to implements their own.
 *  - it needs to be easily usable and clear
 *  - other that I cannot think of right now.
 * Currently we have two implementations :
 *  - Enum : the original implementation
 *  - Union : a new way of placing elements that allows for commands like : IS_FROM_HANDLER() or IS_FROM_VENDOR
 *            it was supposed to be done with unions, but I realized it just provided too much data to the user
              for no reason.
*/

/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # Enum_Impl                                   # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

#define ENUM_MAKE_ID()

typedef u16 enum_id16;
enum {
    ENUM_ID16_GEN = 0b0000'0000'00000000,
    ENUM_ID16_TKN = 0b1000'0000'00000000,
    ENUM_ID16_EVT = 0b0100'0000'00000000,
    ENUM_ID16_ACT = 0b1100'0000'00000000,
    ENUM_ID16_HND = 0b0010'0000'00000000,
};
typedef u32 enum_id32;
enum {
    ENUM_ID32_GEN = (ENUM_ID16_GEN << 15),
    ENUM_ID32_TKN = (ENUM_ID16_TKN << 15),
    ENUM_ID32_EVT = (ENUM_ID16_EVT << 15),
    ENUM_ID32_ACT = (ENUM_ID16_ACT << 15),
    ENUM_ID32_HND = (ENUM_ID16_HND << 15),
};

typedef u16 enum_hid16;
enum {
    ENUM_HID16_CORE             = (ENUM_ID16_HND | 0b0000'00000000),
    ENUM_HID16_LOGGER           = (ENUM_ID16_HND | 0b0000'00000001),
    ENUM_HID16_THREAD           = (ENUM_ID16_HND | 0b0000'00000010),
    ENUM_HID16_WINDOW           = (ENUM_ID16_HND | 0b0000'00000011),
    ENUM_HID16_VULKAN_INSTANCE  = (ENUM_ID16_HND | 0b0001'00000000),
    ENUM_HID16_VULKAN_DEVICES   = (ENUM_ID16_HND | 0b0001'00000001),
    ENUM_HID16_VULKAN_SWAPCHAIN = (ENUM_ID16_HND | 0b0001'00000010),
};
typedef u32 enum_hid32;
enum {
    ENUM_HID32_CORE             = (ENUM_HID16_CORE               << 15),
    ENUM_HID32_LOGGER           = (ENUM_HID16_LOGGER             << 15),
    ENUM_HID32_THREAD           = (ENUM_HID16_THREAD             << 15),
    ENUM_HID32_WINDOW           = (ENUM_HID16_WINDOW             << 15),
    ENUM_HID32_VULKAN_INSTANCE  = (ENUM_HID16_VULKAN_INSTANCE    << 15),
    ENUM_HID32_VULKAN_DEVICES   = (ENUM_HID16_VULKAN_DEVICES     << 15),
    ENUM_HID32_VULKAN_SWAPCHAIN = (ENUM_HID16_VULKAN_SWAPCHAIN   << 15),
};

typedef u32 enum_creator_type;
#define ENUM_CRTTYPE_LOGGER          (ENUM_HID32_LOGGER | 0)
#define ENUM_CRTTYPE_VULKAN_INSTANCE (ENUM_HID32_VULKAN_INSTANCE | 0)
typedef u32 enum_creator_flags;
#define ENUM_CRTFLAG_DEFAULT_CREATOR (1)

typedef u32 enum_getter_type;
#define ENUM_GTRTYPE_LOGGER          (ENUM_HID32_LOGGER | 0)
#define ENUM_GTRTYPE_VULKAN_INSTANCE (ENUM_HID32_VULKAN_INSTANCE | 0)
#define ENUM_CRTTYPE_VULKAN_DEVICES  (ENUM_HID32_VULKAN_DEVICES | 0)
typedef u32 enum_getter_flags;
#define ENUM_GTRFLAG_DIRECT_INSTANCE (1)

/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # Union_Impl                                  # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */
#define UNION_TYPE_MASK         0b11000000
#define UNION_VENDOR_MASK       0b00111111
#define UNION_PROVIDER_MASK     0b1111111100000000
#define UNION_HANDLER_MASK      0b0000000011111111
#define UNION_HID_MASK          0b11111111111111110000000000000000
#define UNION_CREATOR_TYPE_MASK 0b00000000000000001111111111111111

#define UNION_MAKE_PROVIDER(id_type, id_vendor) (u8)    \
    ((id_type << 5) & UNION_TYPE_MASK |                 \
    id_vendor & UNION_VENDOR_MASK)
#define UNION_MAKE_HID(provider, handler_id) (u16)  \
    ((provider << 8) & UNION_PROVIDER_MASK |        \
    handler_id & UNION_HANDLER_MASK)
#define UNION_MAKE_CTYPE(hid, creator_type) (u32)   \
    ((hid << 16) & UNION_HID_MASK |                 \
    creator_type & UNION_CREATOR_TYPE_MASK)

typedef u8 union_provider;
typedef u16 union_hid;
typedef u32 union_ctype;
enum {
    UNION_ID_TYPE_GEN = 0b00000000 & UNION_TYPE_MASK,
    UNION_ID_TYPE_TKN = 0b01000000 & UNION_TYPE_MASK,
    UNION_ID_TYPE_EVT = 0b10000000 & UNION_TYPE_MASK,
    UNION_ID_TYPE_HND = 0b11000000 & UNION_TYPE_MASK,
    UNION_ID_VENDOR_STD = 0 & UNION_VENDOR_MASK,
    UNION_ID_VENDOR_VLK = 1 & UNION_VENDOR_MASK,
    UNION_ID_VENDOR_EXT = 2 & UNION_VENDOR_MASK,

    UNION_HID_LOGGER = UNION_MAKE_HID(
        UNION_MAKE_PROVIDER(UNION_ID_TYPE_HND, UNION_ID_VENDOR_STD),
        1),
    UNION_HID_VULKAN_INSTANCE = UNION_MAKE_HID(
        UNION_MAKE_PROVIDER(UNION_ID_TYPE_HND, UNION_ID_VENDOR_VLK),
        1),
    UNION_HID_VULKAN_DEVICES = UNION_MAKE_HID(
        UNION_MAKE_PROVIDER(UNION_ID_TYPE_HND, UNION_ID_VENDOR_VLK),
        2),

    UNION_CTYPE_LOGGER = UNION_MAKE_CTYPE(
        UNION_HID_LOGGER, 0),
    UNION_CTYPE_VULKAN_INSTANCE = UNION_MAKE_CTYPE(
        UNION_HID_VULKAN_INSTANCE, 0),
    UNION_CTYPE_VULKAN_DEVICES = UNION_MAKE_CTYPE(
        UNION_HID_VULKAN_DEVICES, 0),
};