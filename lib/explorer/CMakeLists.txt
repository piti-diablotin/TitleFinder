
add_library(
  explorer 
  discriminator.cpp
  logger.cpp
  )

add_library(titlefinder::explorer ALIAS explorer)

target_include_directories(
  explorer
  PUBLIC
  ${PROJECT_SOURCE_DIR}/lib
  )

target_link_libraries(
  explorer
  PRIVATE
  )
