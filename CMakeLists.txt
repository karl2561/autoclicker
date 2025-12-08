cmake_minimum_required(VERSION 4.2)

project(Autoclicker)

file(GLOB_RECURSE SOURCES
    "src/*.c"
)

add_executable(autoclicker ${SOURCES})

target_include_directories(autoclicker PRIVATE
    src
    src/utils
)

