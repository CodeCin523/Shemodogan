#ifndef __SHD_VULKAN_H__
#define __SHD_VULKAN_H__

#include "../type.h"

#include <vulkan/vulkan.h>

/* Registration metadata */
extern shd_handler_meta_t SHD_HND_VKINST_META;
static constexpr shd_hid16_t SHD_HND_VKINST_ID = SHD_HID16_VULKAN_INSTANCE;

extern shd_handler_meta_t SHD_HND_VKDEVICES_META;
static constexpr shd_hid16_t SHD_HND_VKDEVICES_ID = SHD_HID16_VULKAN_DEVICES;

/* Runtime data struct */
typedef struct shd_hnd_vkinst {
    shd_basehnd_t base;
    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
} shd_hnd_vkinst_t;

typedef struct shd_hnd_vkdevices {
    shd_basehnd_t base;
    VkPhysicalDevice *phy_devices;
    VkDevice *log_devices;
    VkQueue **queues;

    u8 deviceCount;
} shd_hnd_vkdevices_t;

/* Creator struct */
typedef struct shd_crt_vkinst {
    shd_basecrt_t base;
    const char *appName;
    const char **extensions;

    u32 appVersion;
    u32 extensionCount;

    PFN_vkDebugUtilsMessengerCallbackEXT fCallback;
} shd_crt_vkinst_t;

typedef struct shd_crt_vkdevices {
    shd_basecrt_t base;

    const char **extensions;

    u8 maxDevices;
    u8 maxGraphicQueues, maxComputeQueues, maxTransferQueues;
    u32 extensionCount;

    i32 (*fScoreDevice)(VkPhysicalDevice);
} shd_crt_vkdevices_t;

/* Interface */
VKAPI_ATTR VkBool32 VKAPI_CALL shd_vkinst_default_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
);

i32 shd_vkdevices_default_score_device(VkPhysicalDevice phy_device); 

#endif /* __SHD_VULKAN_H__ */