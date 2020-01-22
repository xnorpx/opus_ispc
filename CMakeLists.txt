cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(opus_ispc CXX)

include(FetchContent)
include(buildtype.cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
set(FETCHCONTENT_QUIET FALSE)

#add_compile_options(/fp:fast)
#--opt=fast-math

# TODO: have a version variable
if (WIN32)
  set(ispc_url http://sourceforge.net/projects/ispcmirror/files/v1.12.0/ispc-v1.12.0-windows.zip)
  set(ispc_binary ispc.exe)
else()
  set(ispc_binary ispc)
  if (APPLE)
    set(ispc_url http://sourceforge.net/projects/ispcmirror/files/v1.12.0/ispc-v1.12.0-macOS.tar.gz)
  else()
    set(ispc_ur http://sourceforge.net/projects/ispcmirror/files/v1.12.0/ispc-v1.12.0-linux.tar.gz)
  endif()
endif()

include(FetchContent)

fetchcontent_declare(ispc_compiler
                     URL
                     ${ispc_url}
                     USES_TERMINAL_DOWNLOAD
                     TRUE
)

FetchContent_Declare(opus
                        GIT_REPOSITORY https://github.com/xiph/opus.git
                        GIT_PROGRESS TRUE
                        USES_TERMINAL_DOWNLOAD
                        TRUE)

FetchContent_Declare(benchmark
                        GIT_REPOSITORY https://github.com/google/benchmark.git
                        GIT_PROGRESS TRUE
                        USES_TERMINAL_DOWNLOAD
                        TRUE)

FetchContent_Declare(googletest
                        GIT_REPOSITORY https://github.com/google/googletest.git
                        GIT_PROGRESS TRUE
                        USES_TERMINAL_DOWNLOAD
                        TRUE)

FetchContent_GetProperties(opus benchmark googletest ispc_compiler)

if(NOT iscp_compiler_POPULATED)
  FetchContent_Populate(ispc_compiler)
  set(ispc ${ispc_compiler_BINARY_DIR}/${ispc_binary})
  configure_file(${ispc_compiler_SOURCE_DIR}/bin/${ispc_binary} ${ispc} COPYONLY)
endif()
add_subdirectory(ispc)

if(NOT opus_POPULATED)
  FetchContent_Populate(opus)
  # maybe we should just consume it not build the full repo here in the test and benchmark
  #add_subdirectory(${opus_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

if(NOT benchmark_POPULATED)
  FetchContent_Populate(benchmark)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
  add_subdirectory(${benchmark_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  add_subdirectory(${googletest_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

enable_testing()

# Move to subfolder

add_executable(opus_ispc_benchmark 
               benchmark/opus_ispc_benchmark.cpp)
target_compile_options(opus_ispc_benchmark PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
     -Werror -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX
     /experimental:external                                                                                                  
     /external:anglebrackets                                                                                                
     /external:W0>)
target_include_directories(opus_ispc_benchmark PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ispc/generated/include) # TODO: yeah I know but I want to start coding not write CMake script for the rest of my life
target_link_libraries(opus_ispc_benchmark PRIVATE benchmark opus_ispc_lib)
install(TARGETS opus_ispc_benchmark RUNTIME DESTINATION "bin" CONFIGURATIONS Release)
add_test(opus_ispc_benchmark opus_ispc_benchmark)

add_executable(opus_ispc_test 
               test/opus_ispc_test.cpp)
target_compile_options(opus_ispc_test PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
     -Werror -pedantic-errors -Wall -Wextra -Wconversion -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX
     /experimental:external                                                                                                  
     /external:anglebrackets                                                                                                
     /external:W0>)
target_include_directories(opus_ispc_test PRIVATE 
    "${googletest_SOURCE_DIR}/googletest/include"
    ispc)

target_include_directories(opus_ispc_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ispc/generated/include) # TODO: yeah I know but I want to start coding not write CMake script for the rest of my life
target_link_libraries(opus_ispc_test PRIVATE gtest gtest_main opus_ispc_lib)
install(TARGETS opus_ispc_test RUNTIME DESTINATION "bin" CONFIGURATIONS Release)
add_test(opus_ispc_test opus_ispc_test)
