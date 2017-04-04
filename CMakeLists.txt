project (CMakeLists C CXX)
set (CMAKE_VERBOSE_MAKEFILE on)

# Tweak build mode
if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
        set(CMAKE_BUILD_RELEASE TRUE)
else()
        set(CMAKE_BUILD_RELEASE FALSE)
endif()

message(STATUS "Building ${PROJECT_NAME} in ${CMAKE_BUILD_TYPE} mode")

cmake_minimum_required(VERSION 3.0.2)
set (CMAKE_VERBOSE_MAKEFILE on)

ENABLE_TESTING()

# Basic build options

#find_package (Threads)

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR APPLE)
  set (CMAKE_CXX_FLAGS "-std=c++14 -fno-exceptions -fno-rtti -Wextra -Ofast -MMD -MP -fpermissive")
endif()

include_directories ("${PROJECT_SOURCE_DIR}/src")

if (MINGW)
  set (CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions -fno-rtti -Wextra -Ofast -MMD -MP -static -static-libgcc -static-libstdc++")
endif()

## silent_majority
file(GLOB_RECURSE SM_HEADERS  src/*.hpp)
file(GLOB_RECURSE SM_SRCS src/*.cpp)

set(SM_ALL_SRCS ${SM_HEADERS} ${SM_SRCS})

set(TARGET godwhale_child)

add_executable(${TARGET} ${SM_ALL_SRCS})
target_link_libraries (${TARGET} ${CMAKE_THREAD_LIBS_INIT})

add_custom_target(avx2)
set(TARGET godwhale_child-avx2)
set(TARGET_OPT  "-DHAVE_SSE4 -DHAVE_SSE42 -DHAVE_AVX2 -mbmi -mbmi2 -mavx2")
add_executable(${TARGET} ${SM_ALL_SRCS})
set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "${TARGET_OPT}")
target_link_libraries (${TARGET} ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(avx2 ${TARGET})

add_custom_target(sse)
set(TARGET godwhale_child-sse)
set(TARGET_OPT  "-DHAVE_SSE4 -DHAVE_SSE42 -msse4.2")
add_executable(${TARGET} ${SM_ALL_SRCS})
set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "${TARGET_OPT}")
target_link_libraries (${TARGET} ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(sse ${TARGET})

add_custom_target(sse2)
set(TARGET godwhale_child-sse2)
set(TARGET_OPT  "-DHAVE_SSE2 -msse2")
add_executable(${TARGET} ${SM_ALL_SRCS})
set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "${TARGET_OPT}")
target_link_libraries (${TARGET} ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(sse2 ${TARGET})
