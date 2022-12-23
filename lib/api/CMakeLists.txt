find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(api
  authentication.cpp
  curl.cpp
  logger.cpp
  search.cpp
  tmdb.cpp
  )

add_library(titlefinder::api ALIAS api)

target_include_directories(api
  PUBLIC
  ${PROJECT_SOURCE_DIR}/lib
  )

target_link_libraries(api
  PUBLIC
  nlohmann_json::nlohmann_json
  PRIVATE
  CURL::libcurl
  spdlog::spdlog
  )
