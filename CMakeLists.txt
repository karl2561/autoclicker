cmake_minimum_required(VERSION 4.2)

project(Autoclicker)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    "src/*.c"
)

add_executable(autoclicker ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(libudev REQUIRED IMPORTED_TARGET libudev)

target_link_libraries(autoclicker
    PRIVATE PkgConfig::libudev
)

target_include_directories(autoclicker
    PRIVATE src
)

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
