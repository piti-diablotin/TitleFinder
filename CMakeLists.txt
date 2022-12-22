cmake_minimum_required(VERSION 3.11)
project(
  TitleFinder
  VERSION 0.0
  LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-Weffc++ CXX_HAS_WEFF)
  check_cxx_compiler_flag(-Wextra CXX_HAS_WEXTRA)
  check_cxx_compiler_flag(-O0 CXX_HAS_NOOPTIM)
  check_cxx_compiler_flag(-Wall CXX_HAS_WALL)
  check_cxx_compiler_flag(-Wno-uninitialized CXX_HAS_WNOINITIALIZED)
  if(CXX_HAS_WEFF)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Weffc++")
  endif()
  if(CXX_HAS_WEXTRA)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra")
  endif()
  if(CXX_HAS_NOOPTIM)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
  endif()
  if(CXX_HAS_WALL)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  endif()
  if(CXX_HAS_WNOINITIALIZED)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-uninitialized")
  endif()
endif()

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(lib)
add_subdirectory(cli)
