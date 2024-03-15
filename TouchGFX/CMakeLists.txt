cmake_minimum_required(VERSION 3.22)

# Setup project if this is top-level
project(TouchGFX)
enable_language(C CXX ASM)

# Root TouchGFX folder - depends on where CMakeLists.txt is located
set(root_tgfx_dir ${CMAKE_CURRENT_LIST_DIR})

# Setup all necessary include directories
# These are used by compiler to search for C or C++ headers
set(touchgfx_include_DIRS

    # Application includes
    ${root_tgfx_dir}/gui/include
    ${root_tgfx_dir}/generated/gui_generated/include
    ${root_tgfx_dir}/generated/images/include
    ${root_tgfx_dir}/generated/bitmaps/include
    ${root_tgfx_dir}/generated/fonts/include
    ${root_tgfx_dir}/generated/texts/include
    ${root_tgfx_dir}/generated/videos/include

    # When running simulator but project started as STM32 project
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include/common
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/common/include

    # When running simulator and project started as simulator project
    ${root_tgfx_dir}/touchgfx/framework/include
    ${root_tgfx_dir}/touchgfx/framework/include/common
    ${root_tgfx_dir}/touchgfx/framework/common/include
    ${root_tgfx_dir}/generated/simulator/include
)

# Search all source files in to be part of the build
# Use the GLOB to scan everything in the gui and generated folders
# These are essential for generic TouchGFX build
file(GLOB_RECURSE graphics_core_generated_SRCS
    ${root_tgfx_dir}/generated/*.cpp
    ${root_tgfx_dir}/gui/*.cpp
    ${root_tgfx_dir}/gui/*.c
)

# Add simulator files and append to current sources
# Simulator files are only needed for simulation build (main file, etc)
file(GLOB_RECURSE glob_tmp ${root_tgfx_dir}/simulator/*.cpp)
set(graphics_core_generated_SRCS ${graphics_core_generated_SRCS} ${glob_tmp})

# Add platform based source files and include them in list of files
# This is used for Win32 build
file(GLOB_RECURSE glob_tmp

    # When running simulator but project started as STM32 project
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/source/platform/*.cpp

    # When running simulator and project started as simulator project
    ${root_tgfx_dir}/touchgfx/framework/source/platform/*.cpp
)
set(graphics_core_generated_SRCS ${graphics_core_generated_SRCS} ${glob_tmp})

# Prepare include paths for the simulator
set(touchgfx_include_DIRS ${touchgfx_include_DIRS}
    ${root_tgfx_dir}/simulator

    # When running simulator but project started as STM32 project
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/mvp/include
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include/platform/hal/simulator/sdl2
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include/platform/hal/simulator/sdl2/vendor
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include/platform/hal/simulator/sdl2/vendor/win32
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include/platform/hal/simulator/sdl2/3rdparty/sdl/include/win32
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/framework/include
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/3rdparty/libjpeg/include

    # When running simulator and project started as simulator project
    ${root_tgfx_dir}/touchgfx/framework/mvp/include
    ${root_tgfx_dir}/touchgfx/framework/include/platform/hal/simulator/sdl2
    ${root_tgfx_dir}/touchgfx/framework/include/platform/hal/simulator/sdl2/vendor
    ${root_tgfx_dir}/touchgfx/framework/include/platform/hal/simulator/sdl2/vendor/win32
    ${root_tgfx_dir}/touchgfx/framework/include/platform/hal/simulator/sdl2/3rdparty/sdl/include/win32
    ${root_tgfx_dir}/touchgfx/framework/include
    ${root_tgfx_dir}/touchgfx/3rdparty/libjpeg/include
)

# Prepare linker directories - for the simulator
set(link_dirs

    # When running simulator but project started as STM32 project
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/lib/win/mingw32
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/lib/sdl2/win32
    ${root_tgfx_dir}/../Middlewares/ST/touchgfx/3rdparty/libjpeg/lib/win32

    # When running simulator and project started as simulator project
    ${root_tgfx_dir}/touchgfx/lib/win/mingw32
    ${root_tgfx_dir}/touchgfx/lib/sdl2/win32
    ${root_tgfx_dir}/touchgfx/3rdparty/libjpeg/lib/win32
)
link_directories(${CMAKE_PROJECT_NAME} ${link_dirs})

# Build executable with sources, include paths and compile definitions
add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${graphics_core_generated_SRCS})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${touchgfx_include_DIRS})
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE WIN32 NDEBUG _CONSOLE SIMULATOR)
# target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    :libtouchgfx.a
    :libSDL2.a
    :libSDL2_image.a
    :libjpeg-8.a
)

# Copy DLL files to the project path
file(GLOB_RECURSE dll_files

    # When running simulator but project started as STM32 project
    ${root_tgfx_dir}/*.dll

    # When running simulator and project started as simulator project
    ${root_tgfx_dir}/../Middlewares/ST/*.dll
)

# Copy all DLL files to build output folder
# Otherwise we won't be able to run the simulation
foreach(dll_file ${dll_files})
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${dll_file} $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)
endforeach()