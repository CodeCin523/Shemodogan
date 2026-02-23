#include <shd/shd.h>
#include <shd/handler/logger.h>
#include <shd/handler/vulkan.h>

#include <stdio.h>
#include <stdlib.h>

int main() {
    shd_status_t status = 0;
    shd_basecrt_t default_creator = {0, SHD_CRTFLAG_DEFAULT_CREATOR};
    

    status = shd_init();
    printf("%d\n", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("failed to init shd.\n");
        return 0;
    }
    printf("success init shd.\n");


    status = shd_register_handler(SHD_HND_LOGGER_ID, &SHD_HND_LOGGER_META);
    printf("%d\n", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("failed to register logger.\n");
        return 0;
    }
    printf("success to register logger.\n");
    status = shd_register_handler(SHD_HND_VKINST_ID, &SHD_HND_VKINST_META);
    printf("%d\n", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("failed to register vkinst.\n");
        return 0;
    }
    printf("success to register vkinst.\n");


    default_creator.type = SHD_CRTTYPE_LOGGER;
    status = shd_handler_initialize(SHD_HND_LOGGER_ID, &default_creator);
    printf("%d\n", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("failed to initialize logger.\n");
        return 0;
    }
    printf("success to initialize logger.\n");

    default_creator.type = SHD_CRTTYPE_VULKAN_INSTANCE;
    status = shd_handler_initialize(SHD_HND_VKINST_ID, &default_creator);
    printf("%d\n", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("failed to initialize vkinst.\n");
        return 0;
    }
    printf("success to initialize vkinst.\n");


    shd_handler_terminate(SHD_HND_VKINST_ID);
    shd_handler_terminate(SHD_HND_LOGGER_ID);
}