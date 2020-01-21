set(ispc_headers_dir ${CMAKE_CURRENT_BINARY_DIR}/generated/include/ispc)
set(ispc_objects_dir ${CMAKE_CURRENT_BINARY_DIR}/generated/objects)
set(ispc_files_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(ispc_headers)
set(ispc_objects)
set(ispc_files)

macro( add_ispc_file )

    set( _OPTIONS_ARGS)
    set( _ONE_VALUE_ARGS FILE TARGETS ARCH)
    set( _MULTI_VALUE_ARGS)

    cmake_parse_arguments( _ISPC "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

    if( _ISPC_FILE )
        message( STATUS "Process ispc file = ${_ISPC_FILE}.ispc" )
        set(ispc_file ${ispc_files_dir}/${_ISPC_FILE}.ispc)
        list(APPEND ispc_files ${ispc_file})
    else()
        message( FATAL_ERROR "ISPC: 'FILE' argument required." )
    endif()

    if( _ISPC_TARGETS )
        message( STATUS "ISPC targets = ${_ISPC_TARGETS}" )
    else()
        message( FATAL_ERROR "ISPC: 'TARGETS' argument required." )
    endif()

    if( _ISPC_ARCH )
        message( STATUS "ISPC arch = ${_ISPC_ARCH}" )
    else()
        message( FATAL_ERROR "ISPC: 'TARGETS' argument required." )
    endif()

    set(ispc_header ${ispc_headers_dir}/${_ISPC_FILE}.h)
    set(ispc_object ${ispc_objects_dir}/${_ISPC_FILE}.obj)

    message(${ispc_header})
    message(${ispc_object})

    # ISPC command
    add_custom_command(OUTPUT ${ispc_header} ${ispc_object}
        COMMAND ${ispc} ${ispc_file} --target=${_ISPC_TARGETS} --arch=${_ISPC_ARCH} -h ${ispc_header} -o ${ispc_object}
        VERBATIM
        COMMENT "ISPC Output: (empty means no errors)"
    )

    message( STATUS "Generated header ${ispc_header}")
    list(APPEND ispc_headers ${ispc_header})
    list(APPEND ispc_objects ${ispc_object})

endmacro()

# TODO: fix so macro can generate multiple targets (i.e. SSE4, avx1 etc)
add_ispc_file(FILE sum TARGETS avx2-i32x16 ARCH x86-64)

add_custom_target(
  generate-ispc
  #DEPENDS ${ispc_headers} ${ispc_objects}
  SOURCES ${ispc_files}
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${ispc_headers} ${ispc_objects})

set_source_files_properties(
  ${ispc_headers} ${ispc_objects}
PROPERTIES GENERATED TRUE
)

add_library(opus_ispc_lib ${ispc_headers} ${ispc_objects})
set_target_properties(opus_ispc_lib PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(opus_ispc_lib generate-ispc)
