# ---- Shemodogan C library ----

set(SHD_SOURCES
    c/shd.c
)

# Static library (safe for Rust FFI)
add_library(shd STATIC ${SHD_SOURCES})
add_library(Shemodogan::SHD ALIAS shd)

# Include directories for cmake / bindgen / Rust headers?
target_include_directories(shd
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Link Vulkan
target_link_libraries(shd
    PUBLIC
        Vulkan::Vulkan
)

# Ensure position-independent code (good practice for static linking)
set_target_properties(shd PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)