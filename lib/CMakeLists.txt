project(
  titlefinder
  VERSION 0.1.0
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
set(API_KEY "" CACHE STRING "TMDB API key;")


find_package(fmt REQUIRED)

# For API
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# For media
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

# For logger
find_package(spdlog REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(api)
add_subdirectory(explorer)
add_subdirectory(logger)
add_subdirectory(media)

set(ALL_SOURCES 
  ${API_SOURCES}
  ${EXPLORER_SOURCES}
  ${LOGGER_SOURCES}
  ${MEDIA_SOURCES}
  )

set(ALL_HEADERS
  ${API_HEADERS}
  ${EXPLORER_HEADERS}
  ${LOGGER_HEADERS}
  ${MEDIA_HEADERS}
  )

add_library(titlefinder
  ${ALL_SOURCES}
  )

add_library(titlefinder::titlefinder ALIAS titlefinder)

target_compile_definitions(titlefinder
  PRIVATE
  TITLEFINDER_VERSION="${PROJECT_VERSION}"
  TITLEFINDER_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  TITLEFINDER_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  TITLEFINDER_VERSION_PATCH=${PROJECT_VERSION_PATCH}
  TITLEFINDER_NAME="${PROJECT_NAME}"
  $<$<BOOL:ioctl_FOUND>:HAVE_IOCTL>
  $<BUILD_INTERFACE:API_KEY="${API_KEY}">
  )

target_include_directories(titlefinder
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/titlefinder>
  )

target_link_libraries(titlefinder
  PUBLIC
  nlohmann_json::nlohmann_json
  PRIVATE
  #api
  CURL::libcurl
  fmt::fmt
  #media
  ${AVUTIL_LIBRARY}
  ${AVCODEC_LIBRARY}
  ${AVFORMAT_LIBRARY}
  #logger
  spdlog::spdlog
  )

set_target_properties(titlefinder PROPERTIES PUBLIC_HEADER
  "${ALL_HEADERS}"
  )

install(
  TARGETS titlefinder
  EXPORT titlefinderTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/titlefinder
  )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/titlefinder/titlefinderConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
  )

configure_file(${PROJECT_SOURCE_DIR}/titlefinderConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/titlefinder/titlefinderConfig.cmake"
  @ONLY
  )

install(EXPORT titlefinderTargets
  NAMESPACE titlefinder::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/titlefinder
  )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/titlefinder/titlefinderConfigVersion.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/titlefinder/titlefinderConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/titlefinder
  )
