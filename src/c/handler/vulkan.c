#include <shd/handler/vulkan.h>
#include <shd/handler/logger.h>
#include <shd/mem/stack.h>
#include <shd/shd.h>

#include <shd/utils/handler.h>

#include <string.h>


extern shd_dfstack_t g_hndtemp_stack;

static shd_hnd_vkinst_t *vkinst_dt = 0;


/* #region VkInst # */
/* ################ */
static VKAPI_ATTR VkBool32 VKAPI_CALL shd_vkinst_default_callback(
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
/* #endregion     # */
/* ################ */

/* #region Handler# */
/* ################ */
static shd_status_t vkinst_init(shd_basecrt_t *_creator) {
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
        VkLayerProperties *availableProps = shd_dfstack_push(&g_hndtemp_stack, propCount * sizeof(VkLayerProperties));
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
    shd_dfstack_clear(&g_hndtemp_stack);

    // Extensions
    const char ** extensions = shd_dfstack_push(&g_hndtemp_stack, creator.extensionCount + 2);
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
    &SHD_HND_LOGGER_ID,
    1,
    sizeof(shd_hnd_vkinst_t)
};
/* #endregion     # */
/* ################ */