#include "shd/shd_type.h"
#include <shd/shd.h>
#include <shd/handler/shd_logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 100000
#define MESSAGE "Benchmark logging message."

static double elapsed_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

// Benchmark a single logger with a given buffer size
static double benchmark_logger(size_t buffer_size) {
    shd_status_t status;

    // Terminate existing logger if initialized
    shd_handler_terminate(SHD_HID16_LOGGER);

    // Initialize logger with the requested buffer
    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, buffer_size };
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    if (status != SHD_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to initialize logger with buffer %zu\n", buffer_size);
        exit(EXIT_FAILURE);
    }

    // Benchmark logging ITERATIONS messages
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        shd_log(MESSAGE, 0);
    }
    shd_log_flush();

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Terminate logger after benchmark
    shd_handler_terminate(SHD_HID16_LOGGER);

    return elapsed_ns(start, end) / ITERATIONS;
}

int main(void) {
    shd_status_t status;

    // Initialize SHD library
    status = shd_init();
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    status = shd_register_handler(SHD_HID16_LOGGER, &shd_handler_logger_meta);
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    // Buffer sizes to test (in bytes)
    size_t buffers[] = { 128, 1024, 8192, 65536, 1024*1024 };
    size_t num_buffers = sizeof(buffers) / sizeof(buffers[0]);
    double results[num_buffers];

    printf("Running %d iterations per buffer size...\n\n", ITERATIONS);

    // Run benchmarks for all buffer sizes
    for (size_t i = 0; i < num_buffers; i++) {
        results[i] = benchmark_logger(buffers[i]);
    }

    // Print summary table at the end
    printf("=== Benchmark Results: Buffer size impact ===\n");
    printf("Buffer size (bytes) | ns/log\n");
    printf("------------------ | --------\n");
    for (size_t i = 0; i < num_buffers; i++) {
        printf("%17zu | %.2f\n", buffers[i], results[i]);
    }

    shd_exit();
    return 0;
}
