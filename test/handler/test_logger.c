#include "shd/shd_type.h"
#include <shd/shd.h>
#include <shd/handler/shd_logger.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Custom assertion for checking status
void assert_success(shd_status_t status, const char *msg) {
    if (status != SHD_STATUS_SUCCESS) {
        printf("Assertion failed: %s\n", msg);
        exit(EXIT_FAILURE);
    }
}

void test_logger_initialization() {
    // Start the SHD library
    shd_status_t status = shd_init();
    assert_success(status, "Failed to initialize SHD library.");

    // Register the logger handler using the provided logger meta
    shd_status_t reg_status = shd_register_handler(SHD_HID16_LOGGER, &shd_handler_logger_meta);
    assert_success(reg_status, "Failed to register logger handler.");

    // Create and initialize logger with a specific buffer size
    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, 8192 }; // Example buffer size
    status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to initialize logger handler.");

    printf("Logger initialized successfully.\n");
}

void test_basic_logging() {
    const char *test_message = "Basic log message.";
    
    // Log an INFO level message
    shd_log(test_message, 0); 
    shd_log_flush(); // Output to stdout

    printf("Logged: %s\n", test_message);
}

void test_buffer_overflow() {
    // Terminate the current logger before reinitializing with a smaller buffer
    shd_handler_terminate(SHD_HID16_LOGGER);
    
    // Create a new logger with a smaller buffer size to force overflow
    shd_crt_logger_t logger_creator = { {SHD_CRTTYPE_LOGGER, 0}, 100 }; // Small buffer size
    shd_status_t status = shd_handler_initialize(SHD_HID16_LOGGER, &logger_creator.base);
    assert_success(status, "Failed to initialize logger handler with a small buffer.");

    const char *long_message = 
        "This is a long logging message that exceeds the buffer size to test the "
        "cutting-off behavior of the logger functionality. It should demonstrate whether "
        "excessively long messages are handled correctly.";

    // Log the long message
    shd_log(long_message, 0); 
    shd_log_flush(); // Output to stdout
    
    printf("Attempted to log long message with truncated behavior.\n");
}

void test_multiple_log_levels() {
    // Assuming the logger has been initialized previously
    const char *messages[] = {
        "Info message.",
        "Debug message.",
        "Warning message.",
        "Error message."
    };

    // Log messages at different levels
    for (int i = 0; i < 4; i++) {
        shd_log(messages[i], i); // Log with different log levels
    }
    
    shd_log_flush(); // Output to stdout
    printf("Logged multiple messages at different levels.\n");
}

void test_null_message_logging() {
    shd_log(NULL, 1); // Log a null message
    shd_log_flush(); // Ensure no crash occurs
    printf("Logged null message safely.\n");
}

void test_logger_termination() {
    // Terminate the logger handler
    shd_status_t status = shd_handler_terminate(SHD_HID16_LOGGER);
    assert_success(status, "Failed to terminate logger handler.");

    // Finalize the SHD library
    status = shd_exit();
    assert_success(status, "Failed to exit SHD library.");
}

int main(void) {
    printf("Running Logger Tests...\n");
    
    // Step through each test
    test_logger_initialization();
    test_basic_logging();
    test_buffer_overflow();
    test_multiple_log_levels();
    test_null_message_logging();
    test_logger_termination();
    
    printf("All tests completed successfully.\n");
    return 0;
}
