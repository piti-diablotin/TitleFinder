project(
  titlefinder_cli
  VERSION 0.2.0
  LANGUAGES CXX
  )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1")
  link_libraries(stdc++fs)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile_commands.json" FORCE)

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

set(CMKAE_BUILD_SHARED ON CACHE BOOL "Enable shared libraries")

find_package(fmt REQUIRED)

include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(sys/ioctl.h ioctl_FOUND)

add_executable(titlefinder_cli
  titlefinder_cli.cpp
  parser.cpp
  application.cpp
  none.cpp
  rename.cpp
  scan.cpp
  search.cpp
  subapp.cpp
  )

target_link_libraries(titlefinder_cli
  PRIVATE
  titlefinder::titlefinder
  fmt::fmt
  )

target_compile_definitions(titlefinder_cli
  PRIVATE
  TITLEFINDER_VERSION="${PROJECT_VERSION}"
  TITLEFINDER_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  TITLEFINDER_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  TITLEFINDER_VERSION_PATCH=${PROJECT_VERSION_PATCH}
  TITLEFINDER_NAME="${PROJECT_NAME}"
  $<$<BOOL:ioctl_FOUND>:HAVE_IOCTL>
  )

include(GNUInstallDirs)

set_target_properties(titlefinder_cli PROPERTIES
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
  INSTALL_RPATH_USE_LINK_PATH TRUE
  )

install(TARGETS titlefinder_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
