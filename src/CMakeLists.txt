# ---- Shemodogan C library ----

# tf, why is this here?
# set(Shemodogan_SOURCES
#     c/shd.c
#     c/handler/shd_logger.c
# )

# Static library (safe for Rust FFI)
add_library(Shemodogan STATIC)
add_library(Shemodogan::Shemodogan ALIAS Shemodogan)

set_target_properties(
    Shemodogan PROPERTIES
        VERSION ${PROJECT_VERSION}
           SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_sources(
    Shemodogan PRIVATE
        c/shd.c
        c/handler/logger.c
        c/handler/vulkan.c
)

set_target_properties(Shemodogan PROPERTIES
    C_STANDARD 23
    C_STANDARD_REQUIRED ON
)

target_include_directories(
    Shemodogan
    PRIVATE ${Shemodogan_SOURCE_DIR}/c
    PUBLIC $<BUILD_INTERFACE:${Shemodogan_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link Vulkan
target_link_libraries(Shemodogan
    PUBLIC
        Vulkan::Vulkan
)