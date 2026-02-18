#define _POSIX_C_SOURCE 199309L

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

static void write_timestamp(char *buf, size_t size) {
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    snprintf(buf, size, "[%04d-%03d %02d:%02d:%02d]",
             tm_now.tm_year + 1900,
             tm_now.tm_yday + 1,
             tm_now.tm_hour,
             tm_now.tm_min,
             tm_now.tm_sec);
}

// --- New test: direct write with write() ---
static double benchmark_direct_preformatted(const char *preformatted) {
    struct timespec start, end;
    size_t len = strlen(preformatted);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < ITERATIONS; i++) {
        write(STDOUT_FILENO, preformatted, len);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    return elapsed_ns(start, end) / ITERATIONS;
}

// --- New test: buffered write using write() ---
static double benchmark_buffered_write(const char *preformatted, size_t buf_size) {
    struct timespec start, end;
    size_t msg_len = strlen(preformatted);
    char *buffer = malloc(buf_size);
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    size_t buf_pos = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < ITERATIONS; i++) {
        if (buf_pos + msg_len > buf_size) {
            write(STDOUT_FILENO, buffer, buf_pos);
            buf_pos = 0;
        }
        memcpy(buffer + buf_pos, preformatted, msg_len);
        buf_pos += msg_len;
    }

    if (buf_pos > 0) {
        write(STDOUT_FILENO, buffer, buf_pos);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    free(buffer);

    return elapsed_ns(start, end) / ITERATIONS;
}

int main(void) {
    printf("Initializing SHD logger...\n");

    shd_status_t status = shd_init();
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    status = shd_register_handler(SHD_HID16_LOGGER, &shd_handler_logger_meta);
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, 1024 * 1024 };
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    if (status != SHD_STATUS_SUCCESS) exit(EXIT_FAILURE);

    printf("Running %d iterations...\n\n", ITERATIONS);

    double time_shd_log = 0.0, time_shd_logh = 0.0;
    double time_fprintf = 0.0, time_snprintf = 0.0;
    double time_localtime_only = 0.0;
    double time_preformatted = 0.0, time_buffered = 0.0;

    // --- Benchmark localtime() only ---
    {
        struct timespec start, end;
        time_t t;
        struct tm tm_now;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < ITERATIONS; i++) {
            time(&t);
            localtime_r(&t, &tm_now);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_localtime_only = elapsed_ns(start, end) / ITERATIONS;
    }

    // --- Benchmark shd_log ---
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < ITERATIONS; i++) {
            shd_log(MESSAGE, 0);
        }
        shd_log_flush();
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_shd_log = elapsed_ns(start, end) / ITERATIONS;
    }

    // --- Benchmark shd_logh ---
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < ITERATIONS; i++) {
            shd_logh("logger", MESSAGE, 0);
        }
        shd_log_flush();
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_shd_logh = elapsed_ns(start, end) / ITERATIONS;
    }

    // --- Benchmark fprintf ---
    {
        char timestamp[32];
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < ITERATIONS; i++) {
            write_timestamp(timestamp, sizeof(timestamp));
            fprintf(stdout, "%s [MSG] - %s\n", timestamp, MESSAGE);
        }
        fflush(stdout);
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_fprintf = elapsed_ns(start, end) / ITERATIONS;
    }

    // --- Preformatted message for new tests ---
    char preformatted[128];
    write_timestamp(preformatted, sizeof(preformatted));
    strcat(preformatted, " [MSG] - ");
    strcat(preformatted, MESSAGE);
    strcat(preformatted, "\n");

    // --- Benchmark direct write using write() ---
    time_preformatted = benchmark_direct_preformatted(preformatted);

    // --- Benchmark buffered write using write() ---
    size_t buffer_size = 8192; // example buffer
    time_buffered = benchmark_buffered_write(preformatted, buffer_size);

    // --- Print summary ---
    printf("\n=== Benchmark Results (ns/log) ===\n");
    printf("localtime() only:       %.2f\n", time_localtime_only);
    printf("shd_log:                %.2f\n", time_shd_log);
    printf("shd_logh:               %.2f\n", time_shd_logh);
    printf("fprintf:                %.2f\n", time_fprintf);
    printf("snprintf:               %.2f\n", time_snprintf);
    printf("direct write:           %.2f\n", time_preformatted);
    printf("buffered write:         %.2f\n", time_buffered);

    shd_handler_terminate(SHD_HID16_LOGGER);
    shd_exit();

    return 0;
}
