cmake_minimum_required(VERSION 3.28)

# ---- Project metadata ----
project(
    Shemodogan
    VERSION 0.0.1
    DESCRIPTION
        "Shemodogan is a handy framework for quickly setting up Vulkan-based projects. It helps streamline the boring parts like initialization and management, so you can jump straight into development."
    LANGUAGES C
)

# ---- Directories ----
set(Shemodogan_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(Shemodogan_SOURCE_DIR  ${CMAKE_CURRENT_LIST_DIR}/src)
set(Shemodogan_TESTS_DIR   ${CMAKE_CURRENT_LIST_DIR}/tests)

message(STATUS "[START] Shemodogan Project Setup")

# ---- CMake settings ----
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Output directories (explicit for Cargo integration) ----
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Options ----
option(Shemodogan_BUILD_EXAMPLES "Build Shemodogan example applications" OFF)
option(Shemodogan_BUILD_TESTS    "Build Shemodogan test applications"    OFF)
option(Shemodogan_RENDERER       "Enable the window handler (SDL3)"     ON)

# ---- Optional user hints (Windows-friendly) ----
set(Shemodogan_VULKAN_ROOT "" CACHE PATH "Vulkan SDK root")
set(Shemodogan_SDL3_ROOT   "" CACHE PATH "SDL3 root")

if(Shemodogan_VULKAN_ROOT)
    list(PREPEND CMAKE_PREFIX_PATH "${Shemodogan_VULKAN_ROOT}")
endif()

if(Shemodogan_SDL3_ROOT)
    list(PREPEND CMAKE_PREFIX_PATH "${Shemodogan_SDL3_ROOT}")
endif()

# ---- Dependencies ----
find_package(Vulkan REQUIRED)
if(Shemodogan_RENDERER)
    find_package(SDL3 REQUIRED CONFIG)
endif()

# ---- Configure headers (optional) ----
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/utils_shdrt.h.in
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/shdrt/utils_shdrt.h
# )

# ---- Subprojects ----
message(STATUS "  [START] Shemodogan library setup")
add_subdirectory(external/memcc)
add_subdirectory(src)

# ---- Install configuration ----
include(GNUInstallDirs)

install(
    TARGETS Shemodogan
    EXPORT ShemodoganTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${Shemodogan_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ---- CMake package config ----
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "ShemodoganConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/ShemodoganConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ShemodoganConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/shemodogan
)

install(
    EXPORT ShemodoganTargets
    FILE ShemodoganTargets.cmake
    NAMESPACE Shemodogan::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/shemodogan
)

# ---- Optional subdirectories ----
if(Shemodogan_BUILD_EXAMPLES)
    message(STATUS "  [START] Shemodogan examples setup")
    add_subdirectory(examples)
endif()

if(Shemodogan_BUILD_TESTS)
    message(STATUS "  [START] Shemodogan tests setup")
    add_subdirectory(tests)
endif()

message(STATUS "[END] Shemodogan Project Setup")