add_library(logger
  logger.cpp
  )

add_library(titlefinder::logger ALIAS logger)

target_include_directories(logger
  PUBLIC
  ${PROJECT_SOURCE_DIR}/lib
  )

target_link_libraries(logger
  PUBLIC
  spdlog::spdlog
  )
