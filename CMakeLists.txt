cmake_minimum_required(VERSION 3.27)

option(USE_ASSIMP "Enable model loading via assimp" OFF)
option(USE_CUDA "Enable CUDA support" OFF)
option(NDEBUG "Disables debug assertions" ON)
option(BUILD_TESTS "Build Catch2 tests" OFF)

if(USE_CUDA)
    project(jtxlib VERSION 1.0.0 LANGUAGES CXX CUDA)
else()
    project(jtxlib VERSION 1.0.0 LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 20)

#region Enable CUDA
if(USE_CUDA)
    include(CheckLanguage)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
        set(CUDA_ENABLED TRUE)

        message(STATUS "[JTXLib] CUDA compiler: ${CMAKE_CUDA_COMPILER}")
        message(STATUS "[JTXLib] CUDA compiler version: ${CMAKE_CUDA_COMPILER_VERSION}")
        message(STATUS "[JTXLib] CUDA include directory: ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")
    else()
        message(FATAL_ERROR "[JTXLib] CUDA compiler not found, but CUDA support was requested")
    endif()
else()
    set(CUDA_ENABLED FALSE)
    message(STATUS "[JTXLib] CUDA support is disabled")
    message(STATUS "[JTXLib] Enabling CPU optimizations")
#    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#        add_compile_options(-O3 -mavx2)
#    elseif(MSVC)
#        add_compile_options(/O2 /arch:AVX2)
#    endif()
endif()
#endregion

#region Check CXX compilation
# Taken from PBRTv4 CMake
include (CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_QUIET ON)
check_cxx_source_compiles ( "
#include <malloc.h>
int main() { void * ptr = _aligned_malloc(1024, 32); }
" HAVE__ALIGNED_MALLOC )
check_cxx_source_compiles ( "
#include <stdlib.h>
int main() {
  void *ptr;
  posix_memalign(&ptr, 32, 1024);
} " HAVE_POSIX_MEMALIGN )
set(CMAKE_REQUIRED_QUIET OFF)

if (HAVE__ALIGNED_MALLOC)
    add_compile_definitions(JTX__ALIGNED_MALLOC)
    message(STATUS "[JTXLib] Using _aligned_malloc() for memory alignment.")
elseif (HAVE_POSIX_MEMALIGN)
    add_compile_definitions(JTX_POSIX_MEMALIGN)
    message(STATUS "[JTXLib] Using posix_memalign() for memory alignment.")
else ()
    message (SEND_ERROR "[JTXLib] No memory alignment function found. Defaulting to memalign().")
endif ()

#endregion

# ASSIMP
if (USE_ASSIMP)
    add_definitions(-USE_ASSIMP)
    message(STATUS "[JTXLib] Enabling assimp scene loading")
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(lib/assimp)
endif()

#region Library Files
set(JTXLIB_MATH
        src/jtxlib/math/vec2.hpp
        src/jtxlib/math/vec3.hpp
        src/jtxlib/math/vec4.hpp
        src/jtxlib/math/vecmath.hpp
        src/jtxlib/math/transform.hpp
        src/jtxlib/math/spherical.hpp
        src/jtxlib/math/spherical.cpp
        src/jtxlib/math/ray.hpp
        src/jtxlib/math/numerical.hpp
        src/jtxlib/math/mat4.cpp
        src/jtxlib/math/mat4.hpp
        src/jtxlib/math/math.hpp
        src/jtxlib/math/bounds.hpp
        src/jtxlib/math/constants.hpp
        src/jtxlib/math/vector.hpp
        src/jtxlib/math/quaternion.hpp
)

set(JTXLIB_SIMD
        src/jtxlib/simd/avxfloat.hpp
)

set(JTXLIB_UTIL
        src/jtxlib/util/assert.hpp
        src/jtxlib/util/taggedptr.hpp
        src/jtxlib/util/rand.hpp
)

set(JTXLIB_CONTAINERS
        src/jtxlib/containers/inlinedvec.hpp
)

set(JTXLIB_STD
        src/jtxlib/jstd/memory_resource.hpp
        src/jtxlib/jstd/memory_resource.cpp
        src/jtxlib/jstd/jstd.hpp
)

set(JTXLIB_HEADERS
        src/jtxlib.hpp
        src/jtxlib/math.hpp
        src/jtxlib/simd.hpp
        src/jtxlib/util.hpp
        src/jtxlib/containers.hpp
        src/jtxlib/mesh/gltf.hpp
        src/jtxlib/mesh/scene.hpp
        src/jtxlib/mesh/scene.cpp
        src/jtxlib/math/dist.hpp
)

add_library(jtxlib STATIC
        ${JTXLIB_MATH}
        ${JTXLIB_SIMD}
        ${JTXLIB_UTIL}
        ${JTXLIB_CONTAINERS}
        ${JTXLIB_STD}
        ${JTXLIB_HEADERS}
)
#endregion

#region jtxlib Properties & Options
set_target_properties(jtxlib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

if(USE_ASSIMP)
    target_link_libraries(jtxlib PRIVATE assimp)
endif()

set(CUDA_ARCH "75" CACHE STRING "CUDA architecture (e.g. 75 for SM 7.5)")
if(CUDA_ENABLED)
    message(STATUS "[JTXLib] Building for CUDA architecture ${CUDA_ARCH}")
    set_target_properties(jtxlib PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            CUDA_ARCHITECTURES ${CUDA_ARCH}
    )
    target_compile_options(jtxlib PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)
    target_compile_definitions(jtxlib PUBLIC CUDA_ENABLED)
endif()
#endregion

#region Include Directories
target_include_directories(jtxlib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if(CUDA_ENABLED)
    target_include_directories(jtxlib PRIVATE ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
endif()
#endregion

#region Tests
if (BUILD_TESTS)
    message(STATUS "[JTXLib] Building tests")
    add_subdirectory("lib/Catch2")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/Catch2/extras)
    include(CTest)
    include(Catch)

    add_subdirectory("tests")
endif ()
#endregion

# Copy over assimp
if (USE_ASSIMP)
    if(WIN32)
        add_custom_command(TARGET jtxlib POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:assimp>
                $<TARGET_FILE_DIR:jtxlib>
        )
    endif()
endif()