#include "api/authentication.hpp"
#include "api/response.hpp"
#include "api/search.hpp"
#include "api/tmdb.hpp"
#include "parser.hpp"

#include <iostream>

int main(int argc, char** argv) {
  Parser parser{argc, argv};
  parser.setOption("help", 'h', "Print help message");
  parser.setOption("api_key", 'k', "API_KEY", "Api key for TheMovieDB");
  parser.setOption("movie", 'm', "Matrix", "Movie title (quoted)");
  parser.setOption("show", 's', "Breaking Bad", "TV Show (quoted)");
  parser.parse();

  if (parser.isSetOption("help")) {
    std::cout << parser << std::endl;
    return 0;
  }

  auto tmdb =
      TitleFinder::Api::Tmdb::create(parser.getOption<std::string>("api_key"));

  TitleFinder::Api::Authentication auth(tmdb);

  auto rep = auth.createRequestToken();
  if (rep->getCode() != 200) {
    auto err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep.get());
    std::cerr << err->getCode() << std::endl;
    std::cerr << err->getMessage() << std::endl;
    return -1;
  }

  TitleFinder::Api::Search search(tmdb);
  if (parser.isSetOption("movie")) {
    rep = search.searchMovies({}, parser.getOption<std::string>("movie"), {},
                              {}, {}, {}, {});

    if (rep->getCode() != 200) {
      auto err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep.get());
      std::cerr << err->getCode() << std::endl;
      std::cerr << err->getMessage() << std::endl;
      return -1;
    }

    auto s =
        dynamic_cast<TitleFinder::Api::Search::SearchMoviesResults*>(rep.get());

    if (s->results.size() > 0) {
      auto first = s->results[0];
      std::cout << first.title.value_or("No title") << " ("
                << first.release_date.value_or("0000-00-00").substr(0, 4) << ")"
                << std::endl;
    }
  }

  if (parser.isSetOption("show")) {
    rep = search.searchTvShows({}, {}, parser.getOption<std::string>("show"),
                               {}, {});

    if (rep->getCode() != 200) {
      auto err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep.get());
      std::cerr << err->getCode() << std::endl;
      std::cerr << err->getMessage() << std::endl;
      return -1;
    }

    auto s = dynamic_cast<TitleFinder::Api::Search::SearchTvShowsResults*>(
        rep.get());

    if (s->results.size() > 0) {
      auto first = s->results[0];
      std::cout << first.name.value_or("No title") << " ("
                << first.first_air_date.value_or("0000-00-00").substr(0, 4)
                << ")" << std::endl;
    }
  }

  return 0;
}
