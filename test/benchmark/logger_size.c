#include "shd/shd_type.h"
#include <shd/shd.h>
#include <shd/handler/shd_logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // for write()

#define ITERATIONS 1000000
#define MESSAGE "Benchmark logging message."

static double elapsed_ns(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

// --- Benchmark SHD logger ---
static double benchmark_logger(size_t buffer_size) {
    shd_status_t status;

    shd_handler_terminate(SHD_HID16_LOGGER);

    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, buffer_size };
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    if (status != SHD_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to initialize logger with buffer %zu\n", buffer_size);
        exit(EXIT_FAILURE);
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < ITERATIONS; i++) {
        shd_log(MESSAGE, 0);
    }
    shd_log_flush();

    clock_gettime(CLOCK_MONOTONIC, &end);

    shd_handler_terminate(SHD_HID16_LOGGER);

    return elapsed_ns(start, end) / ITERATIONS;
}

// --- Preformatted batch benchmark for a given buffer size ---
static double benchmark_preformatted_batch(size_t batch_buffer_size) {
    struct timespec start, end;
    size_t msg_len = strlen(MESSAGE);
    char *buffer = malloc(batch_buffer_size);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    size_t buf_pos = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < ITERATIONS; i++) {
        if (buf_pos + msg_len + 1 > batch_buffer_size) { // +1 for newline
            write(STDOUT_FILENO, buffer, buf_pos);
            buf_pos = 0;
        }
        memcpy(buffer + buf_pos, MESSAGE, msg_len);
        buf_pos += msg_len;
        buffer[buf_pos++] = '\n';
    }

    if (buf_pos > 0) {
        write(STDOUT_FILENO, buffer, buf_pos);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    free(buffer);

    return elapsed_ns(start, end) / ITERATIONS;
}

int main(void) {
    shd_status_t status;

    // Initialize SHD library
    status = shd_init();
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    status = shd_register_handler(SHD_HID16_LOGGER, &shd_handler_logger_meta);
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    // Buffer sizes to test
    size_t buffers[] = { 128, 1024, 8192, 65536, 1024*1024 };
    size_t num_buffers = sizeof(buffers) / sizeof(buffers[0]);
    double shd_results[num_buffers];
    double batch_results[num_buffers];

    printf("Running %d iterations per buffer size...\n\n", ITERATIONS);

    // Benchmark SHD logger and preformatted batch for each buffer
    for (size_t i = 0; i < num_buffers; i++) {
        shd_results[i] = benchmark_logger(buffers[i]);
        batch_results[i] = benchmark_preformatted_batch(buffers[i]);
    }

    // Print summary table
    printf("\n=== Benchmark Results: SHD logger vs Preformatted Batch ===\n");
    printf("Buffer size (bytes) | ns/log (SHD logger) | ns/log (Batch)\n");
    printf("------------------ | ------------------ | ---------------\n");
    for (size_t i = 0; i < num_buffers; i++) {
        printf("%17zu | %18.2f | %13.2f\n",
               buffers[i], shd_results[i], batch_results[i]);
    }

    shd_exit();
    return 0;
}
