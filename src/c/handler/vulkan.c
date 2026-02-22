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
    SHD_HNDIMPL_INITIALIZE_DTINST(SHD_HND_VKINST_ID, shd_hnd_vkinst_t);

    u8 layer_callback = creator.fCallback != 0;

    // Check Validation Layer
    if(layer_callback) {
        const char *VALIDATION_LAYERS[] = {
            "VK_LAYER_KHRONOS_validation"
        };
        const u32 VALIDATION_LAYERS_COUNT = 1;

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
                shd_logh_error(0, "missing vulkan instance layer properties.");
                return SHD_STATUS_FAILED;
            }
        }
    }

    // Instance
    

    SHD_HNDIMPL_INITIALIZE_SUCCESS(vkinst_dt);
}
/* #endregion     # */
/* ################ */