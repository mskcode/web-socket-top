cmake_minimum_required(VERSION 3.20.2)
project(
        web-socket-top-server
        VERSION 0.0.1
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_PARALLEL_LEVEL 4)

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)

#add_subdirectory(libs/glad)
#add_subdirectory(libs/linmath)

add_subdirectory(submodules/fmt)
add_subdirectory(submodules/googletest)
