# ---- Shemodogan C library ----

set(Shemodogan_SOURCES
    c/shd.c
)

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
)

target_compile_features(
    Shemodogan PUBLIC
        c_std_23
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