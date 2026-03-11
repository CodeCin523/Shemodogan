# ---- Shemodogan C library ----

# Create the main static library target
add_library(Shemodogan STATIC)

# Alias for easy referencing
add_library(Shemodogan::Shemodogan ALIAS Shemodogan)

# Set version properties
set_target_properties(
    Shemodogan PROPERTIES
        VERSION     ${PROJECT_VERSION}
        SOVERSION   ${PROJECT_VERSION_MAJOR}
)

# Add sources
target_sources(
    Shemodogan PRIVATE
        c/shd_core.c
)

# Require C23 standard
set_target_properties(Shemodogan PROPERTIES
    C_STANDARD 23
    C_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(
    Shemodogan
    PRIVATE ${Shemodogan_SOURCE_DIR}/c
    PUBLIC
        $<BUILD_INTERFACE:${Shemodogan_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Link dependencies
target_link_libraries(
    Shemodogan
    PUBLIC
        Vulkan::Vulkan
)

# Optional SDL3 support if renderer is enabled
if(Shemodogan_RENDERER)
    target_link_libraries(
        Shemodogan
        PUBLIC SDL3::SDL3
    )
endif()