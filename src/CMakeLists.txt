set(SHD_SOURCES
    shd.c
)

add_library(shd ${SHD_SOURCES})
add_library(Shemodogan::SHD ALIAS shd)

target_include_directories(shd
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(shd
    PUBLIC
        Vulkan::Vulkan
)