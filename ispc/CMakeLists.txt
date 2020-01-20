# ISPC command
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/sum.h ${CMAKE_CURRENT_SOURCE_DIR}/sum.o
    COMMAND ${ispc} ${CMAKE_CURRENT_SOURCE_DIR}/sum.ispc --target=avx2-i32x16 --arch=x86-64 -h ${CMAKE_CURRENT_SOURCE_DIR}/sum.h -o ${CMAKE_CURRENT_SOURCE_DIR}/sum.o
    VERBATIM
    COMMENT "ISPC Output: (empty is good)"
)

add_custom_target(
  generate-ispc
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sum.h ${CMAKE_CURRENT_SOURCE_DIR}/sum.o
)

set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/sum.h ${CMAKE_CURRENT_SOURCE_DIR}/sum.o
PROPERTIES GENERATED TRUE
)

add_library(ispc_lib ${CMAKE_CURRENT_SOURCE_DIR}/sum.o)
set_target_properties(ispc_lib PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(ispc_lib generate-ispc)
