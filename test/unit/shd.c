#include <shd/shd.h>

#include <stdio.h>


shd_status_t Init(shd_basecrt_t *) {
    printf("Initializing...\n");
    return SHD_STATUS_SUCCESS;
}
shd_status_t Term() {
    printf("Terminating...\n");
    return SHD_STATUS_SUCCESS;
}

struct foo_data {
    shd_basehnd_t handler_header;
};
struct bar_data {
    shd_basehnd_t handler_header;
};

u16 deps[] = {100, 101};

shd_handler_meta_t handler_foo = {
    Init, Term, 0,
    "HANDLER_FOO",
    deps,
    0,
    sizeof(struct foo_data)
};
shd_handler_meta_t handler_bar = {
    Init, Term, 0,
    "HANDLER_BAR",
    deps,
    1,
    sizeof(struct bar_data)
};

#ifndef TEST_NOMAIN
int main(void) {
    shd_status_t status;
    shd_basecrt_t creator_header = {
        0, 0
    };
    
    // Init
    status = shd_init();
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed init.\n");
        return 1;
    }
    printf("Success init.\n");

    // Register handlers
    status = shd_register_handler(100, &handler_foo);
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed register foo.\n");
        return 1;
    }
    printf("Success register foo.\n");

    status = shd_register_handler(101, &handler_bar);
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed register foo.\n");
        return 1;
    }
    printf("Success register foo.\n");

    // Initialize handler
    status = shd_handler_initialize(100, &creator_header);
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed initialize foo.\n");
        return 1;
    }
    printf("Success initialize foo.\n");
    
    status = shd_handler_initialize(101, &creator_header);
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed initialize bar.\n");
        return 1;
    }
    printf("Success initialize bar.\n");

    // Terminate handler
    status = shd_handler_terminate(101);
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed terminate bar.\n");
        return 1;
    }
    printf("Success terminate bar.\n");

    status = shd_handler_terminate(100);
    printf("%d - ", status);
    if(status != SHD_STATUS_SUCCESS) {
        printf("Failed terminate foo.\n");
        return 1;
    }
    printf("Success terminate foo.\n");
    
    shd_exit();
    return 0;
}
#endif