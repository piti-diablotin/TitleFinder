#include "api/curl.hpp"
#include "parser.hpp"

#include <iostream>

int main(int argc, char** argv) {
  Parser parser{argc, argv};
  parser.setOption("help", 'h', "Print help message");
  parser.setOption("api_key", 'k', "API_KEY", "Api key for TheMovieDB");
  parser.parse();

  if (parser.isSetOption("help")) {
    std::cout << parser << std::endl;
    return 0;
  }

  TitleFinder::Api::Curl curl("https://httpbin.org");
  auto responseGet = curl.get("/get");

  nlohmann::json data{{"test_string", "hello"}, {"pi", 3.14}, {"bool", true}};
  auto responsePost = curl.post("/post", data);
  auto responseDel = curl.del("/delete", data);

  responseGet.wait();
  auto json = responseGet.get();
  std::cout << json.dump(2) << std::endl;

  responsePost.wait();
  auto r1 = responsePost.get();
  std::cout << "Post returned " << r1 << std::endl;

  responseDel.wait();
  auto r2 = responseDel.get();
  std::cout << "delete returned " << r2 << std::endl;

  return 0;
}
