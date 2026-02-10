cmake_minimum_required(VERSION 3.28)

# ---- Project metadata ----
project(
    Shemodogan
    VERSION 0.0.1
    LANGUAGES C
)

message(STATUS "[START] Shemodogan Project Setup")

# ---- CMake settings ----
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Output directories (explicit for Cargo integration) ----
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Options ----
option(SHD_LIB "Shemodogan Library Setup" ON)
option(SHD_EXAMPLES "Shemodogan Examples Application Setup" OFF)
option(SHD_TEST "Shemodogan Test Application Setup" OFF)
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
if(SHD_LIB)
    message(STATUS "  [START] ShD_Library Setup")
    add_subdirectory(src)
endif()
if(SHD_EXAMPLES)
    message(STATUS "  [START] SHD_Examples Application Setup")
    add_subdirectory(examples)
endif()
if(SHD_TEST)
    message(STATUS "  [START] SHD_Test Application Setup")
    add_subdirectory(test)
endif()

message(STATUS "[END] Shemodogan Project Setup")