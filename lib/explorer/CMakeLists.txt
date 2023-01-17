
add_library(
  explorer 
  discriminator.cpp
  engine.cpp
  logger.cpp
  namefilter.cpp
  )

add_library(titlefinder::explorer ALIAS explorer)

target_include_directories(
  explorer
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/titlefinder>
  )

target_link_libraries(
  explorer
  PRIVATE
  titlefinder::api
  titlefinder::logger
  titlefinder::media
  )

set(EXPLORER_INCLUDES_PUBLIC
  discriminator.hpp
  engine.hpp
  logger.hpp
  namefilter.hpp
  )

set_target_properties(explorer PROPERTIES PUBLIC_HEADER
  "${EXPLORER_INCLUDES_PUBLIC}"
  )

install(
  TARGETS explorer 
  EXPORT titlefinderTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/titlefinder
  )
