cmake_minimum_required (VERSION 2.8.11)
project(PGEFileLibrary C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()
set(CMAKE_CXX_STANDARD 11)
if(MSVC)
    if(MSVC_VERSION GREATER_EQUAL "1900")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
        endif()
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "d")

find_package(Qt5Core)

if(Qt5Core_FOUND)
    set(OPT_DEF_PGEFL_QT_SUPPORT OFF)
else()
    set(OPT_DEF_PGEFL_QT_SUPPORT ON)
endif()

option(PGEFL_QT_SUPPORT "Build also Qt variant" ${OPT_DEF_PGEFL_QT_SUPPORT})
if(PGEFL_QT_SUPPORT)
    message("== PGE-FL Qt Edition WILL be built!")
    if(POLICY CMP0071) # Automoc
        cmake_policy(SET CMP0071 NEW)
    endif()
    # Tell CMake to run moc when necessary:
    set(CMAKE_AUTOMOC ON)

    # As moc files are generated in the binary dir, tell CMake
    # to always look for includes there:
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
else()
    message("== PGE-FL Qt Edition is disabled")
endif()

set(LIBRARY_PROJECT 1)
include(build_props.cmake)
include(pge_file_library.cmake)

set(PGEFL_INSTALLS)

add_library(PGE_FL STATIC
    ${PGE_FILE_LIBRARY_SRCS}
)
list(APPEND PGEFL_INSTALLS PGE_FL)

if(PGEFL_QT_SUPPORT)
    add_library(PGE_FL_QT STATIC
        ${PGE_FILE_LIBRARY_SRCS}
    )
    target_compile_definitions(PGE_FL_QT PUBLIC -DPGE_FILES_QT ${Qt5Core_DEFINITIONS})
    target_include_directories(PGE_FL_QT PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" ${Qt5Core_INCLUDE_DIRS})
    list(APPEND PGEFL_INSTALLS PGE_FL_QT)
endif()

target_include_directories(PGE_FL PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

install(TARGETS ${PGEFL_INSTALLS}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    FRAMEWORK DESTINATION "lib"
    INCLUDES DESTINATION "include"
)

file(GLOB PGE_FL_HEADS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

install(FILES
        ${PGE_FL_HEADS}
        DESTINATION include/PGE_File_Formats
)

# === Unit tests ====
option(WITH_UNIT_TESTS "Enable unit testing" OFF)
if(WITH_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

