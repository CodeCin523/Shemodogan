#include "shd/shd_type.h"
#include <shd/shd.h>
#include <shd/handler/shd_logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_MARKER(name, phase) \
    printf("[TEST:%s:%s]\n", name, phase)

// ------------------------------------------------------------
// Assertion helper (only for critical status checks)
// ------------------------------------------------------------
void assert_success(shd_status_t status, const char *msg) {
    if (status != SHD_STATUS_SUCCESS) {
        printf("ASSERT_FAIL: %s\n", msg);
        exit(EXIT_FAILURE);
    }
}

// ------------------------------------------------------------
// Setup & Teardown
// ------------------------------------------------------------
void test_logger_initialization() {
    TEST_MARKER("LOGGER_INIT", "START");

    shd_status_t status = shd_init();
    assert_success(status, "Failed to initialize SHD library.");

    status = shd_register_handler(SHD_HID16_LOGGER, &shd_handler_logger_meta);
    assert_success(status, "Failed to register logger handler.");

    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, 8192 };
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to initialize logger handler.");

    TEST_MARKER("LOGGER_INIT", "END");
}

void test_logger_termination() {
    TEST_MARKER("LOGGER_TERM", "START");

    shd_status_t status = shd_handler_terminate(SHD_HID16_LOGGER);
    assert_success(status, "Failed to terminate logger handler.");

    status = shd_exit();
    assert_success(status, "Failed to exit SHD library.");

    TEST_MARKER("LOGGER_TERM", "END");
}

// ------------------------------------------------------------
// shd_log Tests
// ------------------------------------------------------------
void test_basic_logging() {
    TEST_MARKER("LOG_BASIC", "START");

    shd_log("Basic log message.", 0);
    shd_log_flush();

    TEST_MARKER("LOG_BASIC", "END");
}

void test_multiple_log_levels() {
    TEST_MARKER("LOG_LEVELS", "START");

    const char *messages[] = {
        "Info message.",
        "Debug message.",
        "Warning message.",
        "Error message."
    };

    for (int i = 0; i < 4; i++) {
        shd_log(messages[i], i);
    }

    shd_log_flush();
    TEST_MARKER("LOG_LEVELS", "END");
}

void test_null_message_logging() {
    TEST_MARKER("LOG_NULL_MSG", "START");

    shd_log(NULL, 1);
    shd_log_flush();

    TEST_MARKER("LOG_NULL_MSG", "END");
}

void test_buffer_overflow() {
    TEST_MARKER("LOG_BUFFER_OVERFLOW", "START");

    shd_handler_terminate(SHD_HID16_LOGGER);

    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, 100 };
    shd_status_t status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to initialize logger handler (small buffer).");

    const char *long_message =
        "This is a long logging message that exceeds the buffer size to test "
        "the cutting-off behavior of the logger functionality. It should "
        "demonstrate safe truncation handling.";

    shd_log(long_message, 0);
    shd_log_flush();

    TEST_MARKER("LOG_BUFFER_OVERFLOW", "END");

    // Restore normal buffer
    shd_handler_terminate(SHD_HID16_LOGGER);
    logger_creator.bufferSize = 8192;
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to restore logger buffer.");
}

// ------------------------------------------------------------
// shd_logh Tests
// ------------------------------------------------------------
void test_logh_basic() {
    TEST_MARKER("LOGH_BASIC", "START");

    shd_logh("logger", "Handler basic message.", 0);
    shd_log_flush();

    TEST_MARKER("LOGH_BASIC", "END");
}

void test_logh_levels() {
    TEST_MARKER("LOGH_LEVELS", "START");

    const char *messages[] = {
        "Handler info.",
        "Handler debug.",
        "Handler warning.",
        "Handler error."
    };

    for (int i = 0; i < 4; i++) {
        shd_logh("logger", messages[i], i);
    }

    shd_log_flush();
    TEST_MARKER("LOGH_LEVELS", "END");
}

void test_logh_unknown_handler() {
    TEST_MARKER("LOGH_UNKNOWN", "START");

    shd_logh("nonexistent", "Unknown handler test.", 1);
    shd_log_flush();

    TEST_MARKER("LOGH_UNKNOWN", "END");
}

void test_logh_null_handler() {
    TEST_MARKER("LOGH_NULL_HANDLER", "START");

    shd_logh(NULL, "Null handler test.", 1);
    shd_log_flush();

    TEST_MARKER("LOGH_NULL_HANDLER", "END");
}

void test_logh_null_message() {
    TEST_MARKER("LOGH_NULL_MSG", "START");

    shd_logh("logger", NULL, 2);
    shd_log_flush();

    TEST_MARKER("LOGH_NULL_MSG", "END");
}

void test_logh_buffer_overflow() {
    TEST_MARKER("LOGH_BUFFER_OVERFLOW", "START");

    // Reinitialize logger with small buffer
    shd_handler_terminate(SHD_HID16_LOGGER);

    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, 100 };
    shd_status_t status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to initialize logger handler (small buffer for logh).");

    const char *long_msg =
        "This is a long handler-based logging message designed to exceed "
        "the configured buffer size and verify truncation behavior.";

    shd_logh("logger", long_msg, 0);
    shd_log_flush();

    TEST_MARKER("LOGH_BUFFER_OVERFLOW", "END");

    // Restore normal buffer
    shd_handler_terminate(SHD_HID16_LOGGER);

    logger_creator.bufferSize = 8192;
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to restore logger buffer after logh overflow test.");
}


// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(void) {
    printf("Running SHD Logger Tests...\n");

    test_logger_initialization();

    // shd_log tests
    test_basic_logging();
    test_multiple_log_levels();
    test_null_message_logging();
    test_buffer_overflow();

    // shd_logh tests
    test_logh_basic();
    test_logh_levels();
    test_logh_unknown_handler();
    test_logh_null_handler();
    test_logh_null_message();
    test_logh_buffer_overflow();

    test_logger_termination();

    printf("All tests completed.\n");
    return 0;
}
