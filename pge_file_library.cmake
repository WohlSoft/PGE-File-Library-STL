# message("Path to PGE File Library is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

set(PGE_FILE_LIBRARY_SRCS)

list(APPEND PGE_FILE_LIBRARY_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/ConvertUTF_PGEFF.c
    ${CMAKE_CURRENT_LIST_DIR}/src/file_formats.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/file_rw_lvl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx38a/file_rw_lvl_38a.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pgex/file_rw_lvlx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pgex/file_rw_meta.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/file_rw_npc_txt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/file_rw_sav.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/file_rw_wld.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pgex/file_rw_wldx.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/file_rw_smbx64_cnf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/file_rwopen.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pgex/file_strlist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/lvl_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/npc_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pgex/pge_x.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/save_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/smbx64.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx64/smbx64_cnf_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wld_filedata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pge_file_lib_globs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/pgex/file_rw_savx.cpp
#    ${CMAKE_CURRENT_LIST_DIR}/src/smbx38a/file_rw_lvl_38a_old.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/smbx38a/file_rw_wld_38a.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/mdx_exception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/value/mdx_value_string.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/value/mdx_value_numeric.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/value/mdx_value_boollist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/value/mdx_value_qstringlist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/mdx_object.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/mdx_section.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/common/mdx_file.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/mdx_meta_file.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mdx/mdx_meta_file_rw.cpp
)

if(PGEFL_ENABLE_RWOPS)
    list(APPEND PGE_FILE_LIBRARY_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/src/pge_file_lib_globs_rwops.cpp
    )
endif()

# Make headers being visible in the tree
file(GLOB HEAD_FILES src/*.hpp src/*.h *.h )
list(APPEND PGE_FILE_LIBRARY_SRCS ${HEAD_FILES})
