#include "api/authentication.hpp"
#include "api/response.hpp"
#include "api/search.hpp"
#include "api/tmdb.hpp"
#include "api/tvseasons.hpp"
#include "parser.hpp"

#include <iostream>

int main(int argc, char** argv) {
  Parser parser{argc, argv};
  parser.setOption("help", 'h', "Print help message");
  parser.setOption("api_key", 'k', "API_KEY", "Api key for TheMovieDB");
  parser.setOption("movie", 'm', "Matrix", "Movie title (quoted)");
  parser.setOption("show", 's', "Breaking Bad", "TV Show (quoted)");
  parser.setOption("info", 'i', "Print additional information");
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
      auto* err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep.get());
      std::cerr << err->getCode() << std::endl;
      std::cerr << err->getMessage() << std::endl;
      return -1;
    }

    auto* s =
        dynamic_cast<TitleFinder::Api::Search::SearchMoviesResults*>(rep.get());

    if (s->results.size() > 0) {
      auto& first = s->results[0];
      std::cout << first.title << " (" << first.release_date.substr(0, 4) << ")"
                << std::endl;
    }
  }

  if (parser.isSetOption("show")) {
    rep = search.searchTvShows({}, {}, parser.getOption<std::string>("show"),
                               {}, {});

    if (rep->getCode() != 200) {
      auto* err = dynamic_cast<TitleFinder::Api::ErrorResponse*>(rep.get());
      std::cerr << err->getCode() << std::endl;
      std::cerr << err->getMessage() << std::endl;
      return -1;
    }

    auto* s = dynamic_cast<TitleFinder::Api::Search::SearchTvShowsResults*>(
        rep.get());

    if (s->results.size() > 0) {
      auto first = s->results[0];
      std::cout << first.name << " (" << first.first_air_date.substr(0, 4)
                << ")" << std::endl;
      if (parser.isSetOption("info")) {
        TitleFinder::Api::TvSeasons tvseasons(tmdb);
        int si = 0;
        while (true) {
          auto details = tvseasons.getDetails(first.id, ++si, {});
          if (details->getCode() == 200) {
            auto* season =
                dynamic_cast<TitleFinder::Api::TvSeasons::DetailsResults*>(
                    details.get());
            int SN = season->season_number;
            for (auto& ep : season->episodes) {
              std::cout << fmt::format("{} S{:02}E{:02} {}", first.name, SN,
                                       ep.episode_number, ep.name)
                        << std::endl;
            }
          } else {
            break;
          }
        }
      }
    }
  }

  return 0;
}
