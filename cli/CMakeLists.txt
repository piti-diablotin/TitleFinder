add_executable(titlefinder_cli
  titlefinder_cli.cpp
  parser.cpp
  )

target_link_libraries(titlefinder_cli
  PRIVATE
  titlefinder::api
  titlefinder::media
  titlefinder::explorer
  fmt::fmt
  )

include(GNUInstallDirs)

set_target_properties(titlefinder_cli PROPERTIES
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
  INSTALL_RPATH_USE_LINK_PATH TRUE
  )

install(TARGETS titlefinder_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
