project(
  TitleFinder_lib
  VERSION 0.1.0
  LANGUAGES CXX
  )

add_subdirectory(api)
add_subdirectory(explorer)
add_subdirectory(logger)
add_subdirectory(media)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/titlefinder/titlefinderConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
  )

configure_file(${CMAKE_SOURCE_DIR}/titlefinderConfig.cmake.in
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
