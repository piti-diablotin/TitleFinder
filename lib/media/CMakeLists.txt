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
  )

add_library(titlefinder::media ALIAS media)

target_include_directories(
  media
  PUBLIC
  ${PROJECT_SOURCE_DIR}
  )

target_link_libraries(
  media
  PRIVATE
  titlefinder::logger
  ${AVUTIL_LIBRARY}
  ${AVCODEC_LIBRARY}
  ${AVFORMAT_LIBRARY}
  )
