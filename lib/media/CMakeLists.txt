find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

add_library(
  media
  file.cpp
  fileinfo.cpp
  logger.cpp
  mkvmux.cpp
  )

add_library(titlefinder::media ALIAS media)

target_include_directories(
  media
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/titlefinder>
  )

target_link_libraries(
  media
  PRIVATE
  titlefinder::logger
  ${AVUTIL_LIBRARY}
  ${AVCODEC_LIBRARY}
  ${AVFORMAT_LIBRARY}
  )

set(MEDIA_INCLUDES_PUBLIC
  file.hpp
  fileinfo.hpp
  logger.hpp
  mkvmux.hpp
  tags.hpp
  )

set_target_properties(media PROPERTIES PUBLIC_HEADER
  "${MEDIA_INCLUDES_PUBLIC}"
  )

install(
  TARGETS media 
  EXPORT titlefinderTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/titlefinder
  )
