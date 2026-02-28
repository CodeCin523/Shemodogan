#include <shd/handler/vulkan.h>
#include <shd/handler/logger.h>
#include <shd/mem/stack.h>
#include <shd/shd.h>

#include <shd/utils/hndimpl.h>

#include <string.h>


static shd_hnd_vkinst_t *vkinst_dt = 0;
static shd_hnd_vkdevices_t *vkdevices_dt = 0;

static const shd_hid16_t vkdeps[] = {SHD_HND_LOGGER_ID, SHD_HND_VKINST_ID, SHD_HND_VKDEVICES_ID};

/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # VkInst                                      # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

VKAPI_ATTR VkBool32 VKAPI_CALL shd_vkinst_default_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    shd_logh(
        "default_callback", pCallbackData->pMessage,
        severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT? 3 : 2
    );
    return VK_FALSE;
}


/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # VkDevices                                   # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

i32 shd_vkdevices_default_score_device(VkPhysicalDevice phy_device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(phy_device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(phy_device, &deviceFeatures);
        
    i32 score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    
    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;
    
    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return -1;
    }
    
    return score;
}


/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # VkInst_Meta                                 # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

static shd_status_t vkinst_init(shd_basecrt_t *_creator) {
    SHD_HNDIMPL_TEMPSTACK_ACCESS(shd_dfstack_t* g_tempstack);
    SHD_HNDIMPL_INITIALIZE_CREATOR(
        _creator, SHD_CRTTYPE_VULKAN_INSTANCE,
        shd_crt_vkinst_t,
        SHD_BUNDLE_ARGS({
            {0,0},
            "App", 0,
            1,
            0,
            shd_vkinst_default_callback
        })
    );
    SHD_HNDIMPL_INITIALIZE_DTINST(vkinst_dt, SHD_HND_VKINST_ID, shd_hnd_vkinst_t);

    u8 layer_callback = creator.fCallback != 0;

    const char *VALIDATION_LAYERS[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    const u32 VALIDATION_LAYERS_COUNT = 1;
    if(layer_callback) { // Check Validation Layer
        u32 propCount = 0;
        vkEnumerateInstanceLayerProperties(&propCount, 0);
        VkLayerProperties *availableProps = shd_dfstack_push(g_tempstack, propCount * sizeof(VkLayerProperties));
        if (!availableProps) return SHD_STATUS_FAILED_INTERN_ALLOC;
        vkEnumerateInstanceLayerProperties(&propCount, availableProps);

        for(u32 layerId = 0; layerId < VALIDATION_LAYERS_COUNT; layerId++) {
            u8 found = 0;
            for(u32 propId = 0; propId < propCount; propId++) {
                if(strcmp(VALIDATION_LAYERS[layerId], availableProps[propId].layerName) != 0)
                    continue;
                found = 1;
                break;
            }

            if(!found) {
                shd_logh_error(SHD_HND_VKINST_META.pName, "missing vulkan instance layer properties.");
                return SHD_STATUS_FAILED;
            }
        }
    }
    shd_dfstack_clear(g_tempstack);

    // Extensions
    const char ** extensions = shd_dfstack_push(g_tempstack, (creator.extensionCount + 2) * sizeof(char *));
    if(!extensions) return SHD_STATUS_FAILED_INTERN_ALLOC;
    u32 extensionCount = 0;
    {
        for(; extensionCount < creator.extensionCount; extensionCount++)
            extensions[extensionCount] = creator.extensions[extensionCount];
        
        // #ifdef onxrt_CONFIG_HANDLER_WINDOW
        //     extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        // #endif
        if(layer_callback)
            extensions[extensionCount++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    // Instance
    VkApplicationInfo app_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO, 
        0,
        creator.appName,
        creator.appVersion,
        "Shemodogan",
        VK_MAKE_VERSION(0,0,1),
        VK_API_VERSION_1_3
    };
    VkInstanceCreateInfo info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        0,
        0,
        &app_info,
        0,
        0,
        extensionCount,
        extensions
    };

    VkDebugUtilsMessengerCreateInfoEXT message_info = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        0,
        0,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        creator.fCallback
    };
    if (layer_callback) {
        info.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        info.ppEnabledLayerNames = VALIDATION_LAYERS;
        info.pNext = &message_info;
    }

    {
        VkResult res = vkCreateInstance(&info, 0, &vkinst_dt->instance);
        if(res != VK_SUCCESS) {
            shd_logh_error(SHD_HND_VKINST_META.pName, "failed initializing VkInstance.");
            return SHD_STATUS_FAILED;
        }
    }

    if(layer_callback) { // Validation Layer
        VkResult res;
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkinst_dt->instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            res = func(vkinst_dt->instance, &message_info, nullptr, &vkinst_dt->messenger);
        else
            res = VK_ERROR_EXTENSION_NOT_PRESENT;

        if(res != VK_SUCCESS) {
            shd_logh_error(SHD_HND_VKINST_META.pName, "failed initializing VkMessager.");
            return SHD_STATUS_FAILED;
        }
    }
    
    shd_logh_info(SHD_HND_VKINST_META.pName, "success initializing.");
    return SHD_STATUS_SUCCESS;
}
static shd_status_t vkinst_term() {
    if(!vkinst_dt)
        return SHD_STATUS_FAILED;

    if(vkinst_dt->messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkinst_dt->instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(vkinst_dt->instance, vkinst_dt->messenger, 0);
        }
        vkinst_dt->messenger = VK_NULL_HANDLE;
    }
    if(vkinst_dt->instance) {
        vkDestroyInstance(vkinst_dt->instance, 0);
        vkinst_dt->instance = VK_NULL_HANDLE;
    }

    vkinst_dt = 0;
    return SHD_STATUS_SUCCESS;
}
static shd_basehnd_t *vkinst_get(shd_basegtr_t *_getter) {
    return 0;
}

shd_handler_meta_t SHD_HND_VKINST_META = {
    vkinst_init,
    vkinst_term,
    vkinst_get,
    "shd_vkinst",
    vkdeps,
    1,
    sizeof(shd_hnd_vkinst_t)
};


/* ##### ##### ##### ##### ##### ##### ##### ##### */
/* # VkDevices_Meta                              # */
/* ##### ##### ##### ##### ##### ##### ##### ##### */

static shd_status_t vkdevice_init(VkPhysicalDevice phy_device, VkDevice *log_device) {
    return SHD_STATUS_FAILED;
}

static shd_status_t vkdevices_init(shd_basecrt_t *_creator) {
    SHD_HNDIMPL_TEMPSTACK_ACCESS(shd_dfstack_t *g_tempstack);
    SHD_HNDIMPL_STATICSTACK_ACCESS(shd_dfstack_t *g_staticstack);

    shd_logh_info(SHD_HND_VKDEVICES_META.pName, "initializing.");

    SHD_HNDIMPL_INITIALIZE_CREATOR(
        _creator, SHD_CRTTYPE_VULKAN_DEVICES,
        shd_crt_vkdevices_t,
        SHD_BUNDLE_ARGS({
            {0,0},
            0,
            1,
            u8_MAX, u8_MAX, u8_MAX,
            0,
            shd_vkdevices_default_score_device
        })
    );
    SHD_HNDIMPL_INITIALIZE_DTINST(vkdevices_dt, SHD_HND_VKDEVICES_ID, shd_hnd_vkdevices_t);

    /* Enumerate Physical Devices */
    u32 deviceCount = 0;
    VkResult res = vkEnumeratePhysicalDevices(vkinst_dt->instance, &deviceCount, 0);
    if(res != VK_SUCCESS || deviceCount == 0) {
        shd_logh_error(SHD_HND_VKDEVICES_META.pName, "no physical devices found.");
        return SHD_STATUS_FAILED;
    }

    shd_logh_info(SHD_HND_VKDEVICES_META.pName, "physical devices found.");

    VkPhysicalDevice *devices =
        shd_dfstack_push(g_tempstack, deviceCount * sizeof(VkPhysicalDevice));
    i32 *scores =
        shd_dfstack_push(g_tempstack, deviceCount * sizeof(i32));

    if(!devices || !scores)
        return SHD_STATUS_FAILED_INTERN_ALLOC;

    vkEnumeratePhysicalDevices(vkinst_dt->instance, &deviceCount, devices);

    vkdevices_dt->deviceCount = 0;

    for(u32 i = 0; i < deviceCount; ++i) {
        scores[i] = creator.fScoreDevice(devices[i]);

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);

        shd_logh_info(
            SHD_HND_VKDEVICES_META.pName,
            props.deviceName
        );

        if(scores[i] > 0)
            vkdevices_dt->deviceCount++;
    }

    if(vkdevices_dt->deviceCount == 0) {
        shd_logh_error(SHD_HND_VKDEVICES_META.pName, "no suitable devices after scoring.");
        return SHD_STATUS_FAILED;
    }

    if(vkdevices_dt->deviceCount > creator.maxDevices)
        vkdevices_dt->deviceCount = creator.maxDevices;

    shd_logh_info(SHD_HND_VKDEVICES_META.pName, "selecting best devices.");

    vkdevices_dt->phy_devices =
        shd_dfstack_push(g_staticstack,
            vkdevices_dt->deviceCount * sizeof(VkPhysicalDevice));

    vkdevices_dt->log_devices =
        shd_dfstack_push(g_staticstack,
            vkdevices_dt->deviceCount * sizeof(VkDevice));

    if(!vkdevices_dt->phy_devices || !vkdevices_dt->log_devices)
        return SHD_STATUS_FAILED_INTERN_ALLOC;

    /* Select Best Devices */
    for(u32 i = 0; i < vkdevices_dt->deviceCount; i++) {
        i32 best_score = -1;
        i32 best_index = -1;

        for(u32 j = 0; j < deviceCount; j++) {
            if(scores[j] > best_score) {
                best_score = scores[j];
                best_index = j;
            }
        }

        if(best_index == -1)
            break;

        vkdevices_dt->phy_devices[i] = devices[best_index];

        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[best_index], &props);

        shd_logh_info(
            SHD_HND_VKDEVICES_META.pName,
            props.deviceName
        );

        scores[best_index] = -1;
    }

    /* Create Logical Devices */
    for(u32 i = 0; i < vkdevices_dt->deviceCount; i++) {
        shd_status_t status =
            vkdevice_init(vkdevices_dt->phy_devices[i],
                          &vkdevices_dt->log_devices[i]);

        if(status != SHD_STATUS_SUCCESS) {
            shd_logh_error(SHD_HND_VKDEVICES_META.pName,
                           "failed creating logical device.");
            return status;
        }

        shd_logh_info(SHD_HND_VKDEVICES_META.pName,
                      "logical device created.");
    }

    shd_logh_info(SHD_HND_VKDEVICES_META.pName,
                  "success initializing.");

    return SHD_STATUS_SUCCESS;
}
static shd_status_t vkdevices_term() {
    SHD_HNDIMPL_STATICSTACK_ACCESS(shd_dfstack_t *g_staticstack);

    if(!vkdevices_dt)
        return SHD_STATUS_FAILED;

    shd_logh_info(SHD_HND_VKDEVICES_META.pName, "terminating.");

    if(vkdevices_dt->log_devices) {
        for(u32 i = 0; i < vkdevices_dt->deviceCount; i++) {
            if(vkdevices_dt->log_devices[i]) {
                vkDestroyDevice(vkdevices_dt->log_devices[i], 0);
                shd_logh_info(
                    SHD_HND_VKDEVICES_META.pName,
                    "logical device destroyed."
                );
            }
        }

        shd_dfstack_pop(g_staticstack, vkdevices_dt->log_devices);
    }

    if(vkdevices_dt->phy_devices) {
        shd_dfstack_pop(g_staticstack, vkdevices_dt->phy_devices);
    }

    vkdevices_dt = 0;

    shd_logh_info(SHD_HND_VKDEVICES_META.pName, "terminated.");
    return SHD_STATUS_SUCCESS;
}
static shd_basehnd_t *vkdevices_get(shd_basegtr_t *_getter) {
    return 0;
}

shd_handler_meta_t SHD_HND_VKDEVICES_META = {
    vkdevices_init,
    vkdevices_term,
    vkdevices_get,
    "shd_vkdevices",
    vkdeps,
    2,
    sizeof(shd_hnd_vkdevices_t)
};