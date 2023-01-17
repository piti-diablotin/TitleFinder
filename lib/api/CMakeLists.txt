find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(api
  authentication.cpp
  curl.cpp
  genres.cpp
  logger.cpp
  search.cpp
  tmdb.cpp
  tv.cpp
  tvseasons.cpp
  )

add_library(titlefinder::api ALIAS api)

target_include_directories(api
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/titlefinder>
  )

target_link_libraries(api
  PUBLIC
  nlohmann_json::nlohmann_json
  PRIVATE
  titlefinder::logger
  CURL::libcurl
  fmt::fmt
  )

set(API_INCLUDES_PUBLIC
  authentication.hpp
  curl.hpp
  exception.hpp
  genres.hpp
  logger.hpp
  optionals.hpp
  response.hpp
  search.hpp
  structs.hpp
  tmdb.hpp
  tv.hpp
  tvseasons.hpp
  )

set_target_properties(api PROPERTIES PUBLIC_HEADER
  "${API_INCLUDES_PUBLIC}"
  )

install(
  TARGETS api
  EXPORT titlefinderTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/titlefinder
  )
