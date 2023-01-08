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
