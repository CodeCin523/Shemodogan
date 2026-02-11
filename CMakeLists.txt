cmake_minimum_required(VERSION 3.28)

# ---- Project metadata ----
project(
    Shemodogan
    VERSION 0.0.1
    DESCRIPTION
        "Shemodogan is a handy framework for quickly setting up Vulkan-based projects. It helps streamline the boring parts like initialization and management, so you can jump straight into development."
    LANGUAGES C
)
set(Shemodogan_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(Shemodogan_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(Shemodogan_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)

message(STATUS "[START] Shemodogan Project Setup")

# ---- CMake settings ----
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Output directories (explicit for Cargo integration) ----
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Options ----
option(Shemodogan_BUILD_EXAMPLES "Shemodogan Examples Application Setup" OFF)
option(Shemodogan_BUILD_TEST "Shemodogan Test Application Setup" OFF)
# option(shd_handlerWindow  "Enable the window handler"  ON)
# option(shd_handlerSetting "Enable the setting handler" ON)
# option(shd_thrSafety      "Enable thread safety"       OFF)

# ---- Optional user hints (Windows-friendly) ----
# Users can pass these on the command line if needed
set(SHD_VULKAN_ROOT "" CACHE PATH "Vulkan SDK root")
# set(SHD_SDL3_ROOT   "" CACHE PATH "SDL3 root")

if(SHD_VULKAN_ROOT)
    list(PREPEND CMAKE_PREFIX_PATH "${SHD_VULKAN_ROOT}")
endif()

# if(SHD_SDL3_ROOT)
#     list(PREPEND CMAKE_PREFIX_PATH "${SHD_SDL3_ROOT}")
# endif()

# ---- Dependencies ----
find_package(Vulkan REQUIRED)
# if(shdrt_handlerWindow)
#     find_package(SDL3 REQUIRED CONFIG)
# endif()

# ---- Configure headers ----
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/utils_shdrt.h.in
#     ${CMAKE_CURRENT_SOURCE_DIR}/include/shdrt/utils_shdrt.h
# )

# ---- Subprojects ----
message(STATUS "  [START] SHD_Library Setup")
add_subdirectory(src)

# include the GNUInstallDirs module to get the canonical install paths defined
include(GNUInstallDirs)
# Install the library and export the CMake targets
install(
    TARGETS Shemodogan
    EXPORT ShemodoganTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
# install the public headers
install(DIRECTORY ${Shemodogan_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# configure the CMake package file so the libray can be included with find_package() later
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "ShemodoganConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/ShemodoganConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ShemodoganConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/shemodogan
)

# install the CMake targets
install(
    EXPORT ShemodoganTargets
    FILE ShemodoganTargets.cmake
    NAMESPACE Shemodogan::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/shemodogan
)

if(Shemodogan_BUILD_EXAMPLES)
    message(STATUS "  [START] SHD_Examples Application Setup")
    add_subdirectory(examples)
endif()
if(Shemodogan_BUILD_TEST)
    message(STATUS "  [START] SHD_Test Application Setup")
    add_subdirectory(test)
endif()

message(STATUS "[END] Shemodogan Project Setup")