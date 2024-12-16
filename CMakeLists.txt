cmake_minimum_required (VERSION 2.8.12...3.5.0)

project(PGEFileLibrary C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

if(POLICY CMP0077) # Allow external configuring when building as sub-directory
    cmake_policy(SET CMP0077 NEW)
endif()

if(POLICY CMP0069) # Allow CMAKE_INTERPROCEDURAL_OPTIMIZATION (lto) to be set
    cmake_policy(SET CMP0069 NEW)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

option(PGEFL_ENABLE_RWOPS "Enable SDL_RWops interfaces (if this option is enabled, then PGE-FL has a build-time dependency on SDL2's headers)" OFF)
option(PGEFL_DISABLE_SMBX38A "Disable loading of SMBX-38A content" OFF)

if(NOT DEFINED PGE_FORCE_QT5 AND NOT DEFINED PGEFL_PREFER_QT5)
    option(PGE_FORCE_QT5 "Try to find Qt5 instead of Qt6 if available" ON)
endif()

macro(pgeflFindQt)
    if(NOT PGE_FORCE_QT5 AND NOT PGEFL_PREFER_QT5)
        find_package(Qt6Core QUIET)
    endif()

    if(PGE_FORCE_QT5 OR PGEFL_PREFER_QT5 OR NOT Qt6Core_FOUND)
        find_package(Qt5Core QUIET)
        set(PGEFL_USE_QT5 TRUE)
        set(PGEFL_QT_CORE_DEFS ${Qt5Core_DEFINITIONS})
        set(PGEFL_QT_CORE_INCS ${Qt5Core_INCLUDE_DIRS})
        message("-- PGE-FL Qt 5 detected")
    else()
        set(PGEFL_USE_QT6 TRUE)
        set(PGEFL_QT_CORE_DEFS ${Qt6Core_DEFINITIONS})
        set(PGEFL_QT_CORE_INCS ${Qt6Core_INCLUDE_DIRS})
        message("-- PGE-FL Qt 6 detected")
    endif()
endmacro()

if(NOT DEFINED PGEFL_QT_SUPPORT AND NOT VITA)
    pgeflFindQt()
endif()

if(PGEFL_QT_SUPPORT)
    pgeflFindQt()
endif()

if(VITA OR PGEFL_USE_QT5 OR PGEFL_USE_QT6)
    set(OPT_DEF_PGEFL_QT_SUPPORT OFF)
else()
    set(OPT_DEF_PGEFL_QT_SUPPORT ON)
endif()

if(NOT PGEFL_QT_SUPPORT)
    option(PGEFL_QT_SUPPORT "Build also Qt variant" ${OPT_DEF_PGEFL_QT_SUPPORT})
endif()
if(PGEFL_QT_SUPPORT AND NOT VITA AND NOT NINTENDO_3DS AND NOT NINTENDO_WII AND NOT NINTENDO_WIIU AND NOT NINTENDO_SWITCH)
    message("== PGE-FL Qt Edition WILL be built!")
    if(POLICY CMP0071) # Automoc
        cmake_policy(SET CMP0071 NEW)
    endif()
    # As moc files are generated in the binary dir, tell CMake
    # to always look for includes there:
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
else()
    message("== PGE-FL Qt Edition is disabled")
endif()

set(LIBRARY_PROJECT 1)
include(build_props.cmake)
include(pge_file_library.cmake)

if(PGEFL_USE_QT6)
    pge_cxx_standard(17)
else()
    pge_cxx_standard(11)
endif()

pgefl_disable_cxx_warning_flag("deprecated-copy" NO_DEPRECATED_COPY)

set(PGEFL_INSTALLS)

add_library(pgefl STATIC
    ${PGE_FILE_LIBRARY_SRCS}
)
set_target_properties(pgefl PROPERTIES AUTOMOC OFF)
target_include_directories(pgefl PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src")
list(APPEND PGEFL_INSTALLS pgefl)

if(PGEFL_ENABLE_RWOPS)
    target_compile_definitions(pgefl PUBLIC -DPGEFL_ENABLE_RWOPS)
endif()

if(PGEFL_DISABLE_SMBX38A)
    target_compile_definitions(pgefl PRIVATE -DPGEFL_DISABLE_SMBX38A)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(pgefl PRIVATE "-Wno-c++20-extensions")
endif()

if(PGEFL_QT_SUPPORT)
    add_library(pgefl_qt STATIC
        ${PGE_FILE_LIBRARY_SRCS}
    )
    set_target_properties(pgefl_qt PROPERTIES AUTOMOC ON)
    target_compile_definitions(pgefl_qt PUBLIC -DPGE_FILES_QT ${PGEFL_QT_CORE_DEFS})
    target_include_directories(pgefl_qt PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        ${PGEFL_QT_CORE_INCS})
    list(APPEND PGEFL_INSTALLS pgefl_qt)

    if(PGEFL_ENABLE_RWOPS)
        target_compile_definitions(pgefl_qt PUBLIC -DPGEFL_ENABLE_RWOPS)
    endif()

    if(PGEFL_DISABLE_SMBX38A)
        target_compile_definitions(pgefl PRIVATE -DPGEFL_DISABLE_SMBX38A)
    endif()
endif()

# Don't install libraries when PGE-FL was built as a part of Moondust master project
if(NOT Moondust_SOURCE_DIR)
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
endif()

# === Unit tests ====
option(WITH_UNIT_TESTS "Enable unit testing" OFF)
if(WITH_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
