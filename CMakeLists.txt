cmake_minimum_required (VERSION 2.8.11)
project(PGEFileLibrary C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    message("== Using default build configuration which is a Release!")
endif()

if(POLICY CMP0077) # Allow external configuring when building as sub-directory
    cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX "d")

find_package(Qt5Core QUIET)

if(Qt5Core_FOUND)
    set(OPT_DEF_PGEFL_QT_SUPPORT OFF)
else()
    set(OPT_DEF_PGEFL_QT_SUPPORT ON)
endif()

if(NOT PGEFL_QT_SUPPORT)
    option(PGEFL_QT_SUPPORT "Build also Qt variant" ${OPT_DEF_PGEFL_QT_SUPPORT})
endif()
if(PGEFL_QT_SUPPORT)
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

pge_cxx_standard(11)

set(PGEFL_INSTALLS)

add_library(pgefl STATIC
    ${PGE_FILE_LIBRARY_SRCS}
)
set_target_properties(pgefl PROPERTIES AUTOMOC OFF)
target_include_directories(pgefl PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND PGEFL_INSTALLS pgefl)

if(PGEFL_QT_SUPPORT)
    add_library(pgefl_qt STATIC
        ${PGE_FILE_LIBRARY_SRCS}
    )
    set_target_properties(pgefl_qt PROPERTIES AUTOMOC ON)
    target_compile_definitions(pgefl_qt PUBLIC -DPGE_FILES_QT ${Qt5Core_DEFINITIONS})
    target_include_directories(pgefl_qt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" ${Qt5Core_INCLUDE_DIRS})
    list(APPEND PGEFL_INSTALLS pgefl_qt)
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

