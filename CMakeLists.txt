cmake_minimum_required(VERSION 3.10)
project(DigitalMediaPlayer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(DigitalMediaPlayer main.cpp ${SOURCES})

target_include_directories(DigitalMediaPlayer PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(DigitalMediaPlayer PRIVATE -Wall -Wextra -Wpedantic)
    # -Werror is opt-in via -DSTRICT_WARNINGS=ON so CI/instructor machines
    # with slightly different toolchains still build; the project itself
    # aims for zero warnings regardless.
    option(STRICT_WARNINGS "Treat warnings as errors" OFF)
    if(STRICT_WARNINGS)
        target_compile_options(DigitalMediaPlayer PRIVATE -Werror)
    endif()
endif()

# Copy sample data next to the executable so `data/media.txt` resolves
# whether you run from the build directory or the project root.
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})

enable_testing()
add_subdirectory(tests)
