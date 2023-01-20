project(
  titlefinder_cli
  VERSION 0.2.0
  LANGUAGES CXX
  )

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
  titlefinder::explorer
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
