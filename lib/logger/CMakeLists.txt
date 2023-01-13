add_library(logger
  logger.cpp
  )

add_library(titlefinder::logger ALIAS logger)

target_include_directories(logger
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/titlefinder>
  )

target_link_libraries(logger
  PUBLIC
  spdlog::spdlog
  )

set(LOGGER_INCLUDES_PUBLIC
  logger.hpp
  )

set_target_properties(logger PROPERTIES PUBLIC_HEADER
  "${LOGGER_INCLUDES_PUBLIC}"
  )

install(
  TARGETS logger 
  EXPORT titlefinderTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/titlefinder
  )
