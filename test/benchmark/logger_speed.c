#define _POSIX_C_SOURCE 199309L

#include "shd/shd_type.h"
#include <shd/shd.h>
#include <shd/handler/shd_logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    // --- Benchmark snprintf ---
    /* {
        char buffer[256];
        char timestamp[32];
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < ITERATIONS; i++) {
            write_timestamp(timestamp, sizeof(timestamp));
            snprintf(buffer, sizeof(buffer), "%s [MSG] - %s\n", timestamp, MESSAGE);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_snprintf = elapsed_ns(start, end) / ITERATIONS;
    } */

    // --- Print summary ---
    printf("\n=== Benchmark Results (ns/log) ===\n");
    printf("localtime() only: %.2f\n", time_localtime_only);
    printf("shd_log:         %.2f\n", time_shd_log);
    printf("shd_logh:        %.2f\n", time_shd_logh);
    printf("fprintf:         %.2f\n", time_fprintf);
    printf("snprintf:        %.2f\n", time_snprintf);

    shd_handler_terminate(SHD_HID16_LOGGER);
    shd_exit();

    return 0;
}
