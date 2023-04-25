cmake_minimum_required(VERSION 3.2)
set(PROJECT Engine1)
project(Engine1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

set(GCC_COVERAGE_COMPILE_FLAGS "-std=c++17" "-Wextra" "-Wall" "-Werror" "-Wpedantic" "-ggdb")
set(GCC_COVERAGE_LINK_FLAGS "-lglfw" "-lvulkan" "-ldl" "-lpthread" "-lX11" "-lXxf86vm" "-lXrandr" "-lXi" "-pthread")

option(MSG "ON or OFF" OFF)

#if(MSG)
#    message("Option is on")
#else()
#    message("Option is off")
#endif()

# Add all source files
set(SOURCE_FILES 

    src/main.cpp 

    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Allocator.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Allocator.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/File.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/File.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/String.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/String.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/HashMap.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/hashmap_util.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common/Vec.hpp" 

    "${CMAKE_CURRENT_SOURCE_DIR}/src/vulk/SpirvParser.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/vulk/SpirvParser.cpp" 

    "${CMAKE_CURRENT_SOURCE_DIR}/src/external/stb_image.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external/stb_image.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external/tlsf.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external/tlsf.h" 
)

add_executable(Engine1 ${SOURCE_FILES})

target_compile_options(Engine1 PRIVATE "${GCC_COVERAGE_COMPILE_FLAGS}")
target_link_libraries(Engine1 PRIVATE "${GCC_COVERAGE_LINK_FLAGS}")

target_include_directories(Engine1 PUBLIC 
    "${PROJECT_BINARY_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/external"
)














