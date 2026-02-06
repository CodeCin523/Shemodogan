cmake_minimum_required(VERSION 3.28)

# ---- Project metadata ----
project(
    Shemodogan
    VERSION 0.0.1
    LANGUAGES C
)

message(STATUS "[START] SHD project setup")

# ---- CMake settings ----
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Options ----
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
if(NOT DEFINED shdl OR shdl)
    message(STATUS "  [START] SHDLibrary setup")
    add_subdirectory(src shdl)
endif()

if(DEFINED shde AND shde)
    message(STATUS "  [START] SHDExamples applications setup")
    add_subdirectory(examples shde)
endif()

if(DEFINED shdt AND shdt)
    message(STATUS "  [START] SHDTests applications setup")
    add_subdirectory(tests shdt)
endif()

message(STATUS "[END] SHD project setup")